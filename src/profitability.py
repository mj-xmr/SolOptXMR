from kraken import coin, fiat, kraken
from datetime import date, datetime, timedelta
import pandas as pd
import requests
import warnings
import math

from python_json_config import ConfigBuilder

config_builder = ConfigBuilder()
config = config_builder.parse_config('config.json')
dict_config = config.to_dict()
DIR_TMP = config.sunrise_lib.DIR_TMP


class POW_Coin:
    def __init__(self, coin : coin):
        # TODO: figure out a way to use the abstract base class as a dispatcher for the specialized
        # subclasses/overridden methods based on the "coin" enum
        # TODO: implement daemon RPC calls using monero-python (pip install monero)
        now = datetime.now()
        self.coin = coin
        self.node_url = dict_config["profitability"][coin.name]["node"]
        self.blocktime = dict_config["profitability"][coin.name]["blocktime"]
        self._height = None
        self._height_last_fetched = now
        self._price = None
        self._price_last_fetched = now
        self._difficulty = None
        self._difficulty_last_fetched = now
        self._reward = None
        self._reward_last_fetched = now
    
    @property
    def price(self) -> float:
        now = datetime.now()
        if self._price is None or now - self._price_last_fetched > timedelta(minutes=config.profitability.refresh_delta_mins):
            self._price = kraken.get_prices(self.coin)
            self._price_last_fetched = now
        return self._price
    
    @property
    def difficulty(self) -> int:
        now = datetime.now()
        if self._difficulty is None or now - self._difficulty_last_fetched > timedelta(minutes=config.profitability.refresh_delta_mins):
            self.get_info()
        return self._difficulty
    
    @property
    def height(self) -> int:
        now = datetime.now()
        if self._height is None or now - self._height_last_fetched > timedelta(minutes=config.profitability.refresh_delta_mins):
            self.get_info()
        return self._height
    
    @property
    def reward(self) -> float:
        now = datetime.now()
        if self._reward is None or now - self._reward_last_fetched > timedelta(minutes=config.profitability.refresh_delta_mins):
            req_data = {"jsonrpc": "2.0", "id": "0", "method": "get_last_block_header"}
            data = requests.post(f"{self.node_url}/json_rpc", json=req_data).json()
            self._reward = int(data["result"]["block_header"]["reward"]) / 1e12
            self._reward_last_fetched = now
        return self._reward
    
    def profitability(self, fiat: fiat, hashrate: int, power_consumption: int, electricity_cost: float, pool_fee:float=0, price:float=None, reward:float=None, difficulty:int=None):
        if pool_fee < 0 or pool_fee > 1:
            raise ValueError("Invalid pool fee!")
        # nethash = self.difficulty / self.blocktime
        data = {
            "coin": self.coin.name,
            "difficulty": difficulty if difficulty else self.difficulty,
            "price": price if price else self.price[fiat],
            "reward": reward if reward else self.reward,
            "efficiency": hashrate / power_consumption if power_consumption != 0 else math.inf,
            "mining_cost_s": power_consumption * electricity_cost / 1000 / 3600,  # W * fiat/(kW*h) / 1000 W/kW / 3600 s/h = fiat/s
        }
        data["expected_crypto_income_s"] = data["reward"] * hashrate / data["difficulty"] * (1 - pool_fee)
        data["emission_rate_s"] = data["price"] * data["reward"] / self.blocktime
        data["expected_fiat_income_s"] = data["price"] * data["expected_crypto_income_s"]
        data["expected_fiat_income_kwh"] = data["efficiency"] * data["reward"] * data["price"] * (1 - pool_fee) * 1000 * 3600 / data["difficulty"]
        data["profitability"] = 100 * (data["expected_fiat_income_s"] - data["mining_cost_s"]) / data["mining_cost_s"] if data["mining_cost_s"] != 0 else math.inf
        data["breakeven_efficiency"] = (data["difficulty"] * electricity_cost) / (data["price"] * data["reward"] * 1000 * 3600 * (1 - pool_fee))
        return data
    
    def get_info(self) -> dict:
        now = datetime.now()
        json_req = { "jsonrpc": "2.0", "id": "0", "method": "get_info"}
        try:
            data = requests.get(f"{self.node_url}/json_rpc", json=json_req).json()
        except Exception as e:
            print("Error!", e)
            pass
        else:
            self._height = data["result"]["height"]
            self._height_last_fetched = now
            self._difficulty = data["result"]["difficulty"]
            self._difficulty_last_fetched = now
        return data["result"]
    
    def _request_headers_range(self, start_height:int, end_height:int) -> pd.DataFrame:
        json_req = {
            "jsonrpc": "2.0",
            "id": "0",
            "method": "get_block_headers_range",
            "params": {"start_height": start_height, "end_height": end_height}
        }
        data = requests.post(f"{self.node_url}/json_rpc", json=json_req).json()
        result = pd.DataFrame(data["result"]["headers"], columns=["height", "timestamp", "difficulty", "reward"])
        result.set_index("height", inplace=True)
        return result
    
    def _request_headers_batcher(self, start_height:int, end_height:int, batch_size:int=1000) -> tuple[pd.DataFrame, bool]:
        # if (end_height - start_height) >= batch_size, send a batch request then
        # evaluate again with start_height = start_height + batch_size
        # if it's false request the last range from start_height to end_height
        results = []
        try:
            while (end_height - start_height >= batch_size):
                print(f"Downloading headers from {start_height} to {start_height + batch_size - 1}")
                results.append(self._request_headers_range(start_height, start_height + batch_size - 1))
                start_height = start_height + batch_size
            print(f"Downloading headers from {start_height} to {end_height}")
            results.append(self._request_headers_range(start_height, end_height))
        except KeyboardInterrupt:
            if results == []:
                print("Aborting block data fetch")
            else:
                print("Stopped - saving last data fetched...")
            try:
                result = pd.concat(results, axis=0)
            except ValueError:  # ValueError: No objects to concatenate - if intrerrupting during the first fetch
                return None, True
            else:
                return result, True  # Ugly, but necessary to exit the while loop in historical_diff
        else:
            result = pd.concat(results, axis=0)
            return result, False
    
    def historical_diff(self, height:int=None, timestamp:datetime=None, batch_size:int=1000) -> int:
        if height and timestamp:
            warnings.warn("Both height and timestamp present: ignoring timestamp", UserWarning)
        
        path = f"{DIR_TMP}/diff_{self.coin.name}.pkl"
        try: # If we have previous saved data, merge with the new data
            diff = pd.read_pickle(path)
        except FileNotFoundError:
            print(f"{path} does not exist. Creating...")
            diff = self._request_headers_batcher(0, height, batch_size=batch_size)
            diff.to_pickle(path)
            print(diff)
            pass
        
        last_known_height = int(diff.index[-1])
        last_known_timestamp = datetime.fromtimestamp(diff["timestamp"].iloc[-1])
        
        if height:
            if height > self.height:
                raise ValueError("Requested height is in the future")
            if height < 0:
                raise ValueError("Cannot have a negative height")
            if height > last_known_height:  # Need to update
                # diff_new = self._request_headers_batcher(last_known_block + 1, self.height, batch_size=batch_size)
                diff_new = self._request_headers_batcher(last_known_height + 1, height, batch_size=batch_size)
                if diff_new is not None:
                    diff = pd.concat([diff, diff_new], axis=0)
                    diff.to_pickle(path)
                else:
                    raise KeyboardInterrupt
            try:
                return diff.at[height, "difficulty"]
            except KeyError:
                print(f"Block {height} missing from historical data cache!")
        elif timestamp:
            dt_timestamp = datetime.fromtimestamp(timestamp)
            if dt_timestamp > datetime.now():
                raise ValueError("Requested timestamp is in the future")
            if dt_timestamp < datetime.fromtimestamp(0):
                raise ValueError("Cannot have a negative timestamp")
            while dt_timestamp > last_known_timestamp and last_known_height < self.height:  # Need to update
                xmr_blocktime_120_height = 1009827
                xmr_blocktime_120_ts = 1458748658
                xmr_blocktime_120_dt = datetime.fromtimestamp(xmr_blocktime_120_ts)
                if dt_timestamp < xmr_blocktime_120_dt:
                    # print(math.ceil((dt_timestamp - last_known_timestamp).total_seconds() / 60))
                    target_height = min(self.height, last_known_height + math.ceil((dt_timestamp - last_known_timestamp).total_seconds() / 60))
                else:
                    # print(math.ceil((dt_timestamp - xmr_blocktime_120_dt).total_seconds() / 120))
                    target_height = min(self.height, xmr_blocktime_120_height + math.ceil((dt_timestamp - xmr_blocktime_120_dt).total_seconds() / 120))
                # diff_new = self._request_headers_batcher(last_known_height + 1, self.height, batch_size=batch_size)
                diff_new, blocked = self._request_headers_batcher(last_known_height + 1, target_height, batch_size=batch_size)
                if diff_new is not None:
                    diff = pd.concat([diff, diff_new], axis=0)
                    diff.to_pickle(path)
                    if blocked:
                        raise KeyboardInterrupt
                    last_known_height = int(diff.index[-1])
                    last_known_timestamp = datetime.fromtimestamp(diff["timestamp"].iloc[-1])
                else:
                    raise KeyboardInterrupt
            # search timestamps and find nearest-previous height index
            res = diff.loc[diff["timestamp"] <= timestamp]
            return res["difficulty"].iloc[-1]
        else:  # not height and not timestamp:
            raise TypeError("Need a height or a timestamp")
    
    def check_block_pickle_integrity(self) -> bool:
        path = f"{DIR_TMP}/diff_{self.coin.name}.pkl"
        try: # If we have previous saved data, merge with the new data
            diff:pd.DataFrame = pd.read_pickle(path)
        except FileNotFoundError:
            print(f"{path} does not exist. Creating...")
            return False
        length, _ = diff.shape
        last_known_height = int(diff.index[-1])
        if length == last_known_height + 1:  # Height includes block #0, so off-by-one
            return True
        else:
            print("Block cache contains duplicates!")
            print("Length:", length)
            print("Last known height:", last_known_height)
            return False


def test():
    import time
    a = POW_Coin(coin.XMR)
    b = a.profitability(fiat.USD, 20000, 200, 0.1)
    print(b)
    assert type(a.difficulty) == int
    print("Difficulty:", a.difficulty)
    a1 = a._difficulty_last_fetched
    # print(a1)
    time.sleep(2)
    a2 = a._difficulty_last_fetched
    print(a2)  # Must match a1
    assert type(a1) == datetime
    assert a1 == a2
    print("Height:", a.height)
    assert type(a.height) == int
    print(a.get_info())
    # h1 = a._request_headers_range(2500000,2500004)
    # h2 = a._request_headers_range(2500003,2500005)
    # h3 = pd.concat([h1, h2], axis=0)
    # h4 = h1.combine_first(h2)
    # print(h1)
    # print(h2)
    # print(h3)
    # print(h4)
    # h5 = a._request_headers_batcher(2500000, 2500001, batch_size=1)  # TODO: assert that it is performed in 2 batches
    # print(h5)
    assert a.historical_diff(height=10) == 21898
    assert a.historical_diff(height=69420) == 237475428
    assert a.historical_diff(timestamp=1397818225) == 27908  # Block 5
    assert a.historical_diff(timestamp=1448116661) == 861110356  # Block 835786
    assert a.historical_diff(timestamp=1497817416) == 9690685763  # Block 1335437
    assert a.check_block_pickle_integrity() == True

if __name__ == "__main__":
    test()
