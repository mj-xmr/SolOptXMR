from kraken import coin, fiat, kraken
from datetime import date, datetime, timedelta
import requests
import math

from python_json_config import ConfigBuilder

config_builder = ConfigBuilder()
config = config_builder.parse_config('config.json')
dict_config = config.to_dict()


class POW_Coin:
    def __init__(self, coin : coin):
        # TODO: figure out a way to use the abstract base class as a dispatcher for the specialized
        # subclasses/overridden methods based on the "coin" enum
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
    def price(self):
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
    def reward(self):
        now = datetime.now()
        if self._reward is None or now - self._reward_last_fetched > timedelta(minutes=config.profitability.refresh_delta_mins):
            req_data = {"jsonrpc": "2.0", "id": "0", "method": "get_last_block_header"}
            data = requests.post(f"{self.node_url}/json_rpc", json=req_data).json()
            self._reward = int(data["result"]["block_header"]["reward"]) / 1e12
            self._reward_last_fetched = now
        return self._reward
    
    def profitability(self, fiat: fiat, hashrate: int, power_consumption: int, electricity_cost: float, pool_fee=0, price=None, reward=None, difficulty=None):
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
    
    def get_info(self):
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
    
def test():
    import time
    a = POW_Coin(coin.XMR)
    b = a.profitability(fiat.USD, 20000, 200, 0.1)
    print(b)
    print(a.difficulty)
    print(a._difficulty_last_fetched)
    time.sleep(2)
    print(a.height)
    print(a._height_last_fetched)  # Must match a._difficulty_last_fetched

if __name__ == "__main__":
    test()
