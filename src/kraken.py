from pykrakenapi import KrakenAPI
from pykrakenapi.pykrakenapi import KrakenAPIError
from datetime import datetime
from enum import Enum, auto
import krakenex

import sunrise_lib

config = sunrise_lib.config
DATE_NOW = sunrise_lib.DATE_NOW
DIR_TMP = sunrise_lib.DIR_TMP

class coin(Enum):
    XMR = auto()


class fiat(Enum):
    EUR = auto()
    USD = auto()


class kraken:
    api = krakenex.API()
    k = KrakenAPI(api, retry=0.5, crl_sleep=1)
    pairs = {
        fiat.EUR: {
            coin.XMR: "XXMRZEUR",
        },
        fiat.USD: {
            coin.XMR: "XXMRZUSD",
        }
    }

    @staticmethod
    def get_price(co: coin, fi: fiat) -> float:
        pair = kraken.pairs[fi][co]
        data = kraken.k.get_ticker_information(pair=pair)
        coin_price = float(data.at[pair, "c"][0])
        return coin_price

    @staticmethod
    def get_prices(co: coin) -> dict:
        prices = {}
        pairs_to_fetch = ""
        for fi in fiat:
            pairs_to_fetch = pairs_to_fetch + "," + kraken.pairs[fi][co]
        data = kraken.k.get_ticker_information(pair=pairs_to_fetch[1:])  # Remove first comma
        for fi in fiat:
            pair = kraken.pairs[fi][co]
            prices[fi] = float(data.at[pair, "c"][0])
        return prices

    def get_ohlc(co:coin, fi:fiat, interval:int=1, since:int=None, ascending:bool=True):
        if interval not in [1, 5, 15, 30, 60, 240, 1440, 10080, 21600]:
            raise ValueError("Invalid interval")
        pair = kraken.pairs[fi][co]
        ohlc, last = kraken.k.get_ohlc_data(pair, interval=interval, since=since, ascending=ascending)
        return ohlc, last

def test(year=DATE_NOW.year, month=DATE_NOW.month, day=DATE_NOW.day):
    import pandas as pd
    fi = fiat.EUR
    co = coin.XMR
    try:
        a = kraken.get_price(co, fi)
    except Exception as e:
        # print(f"Could not fetch price for {kraken.pairs[fi][co]}. {e}")
        print(f"Could not fetch price for {kraken.pairs[fi][co]}.")
    else:
        print(a)
    try:
        b = kraken.get_prices(co)
    except Exception as e:
        # print(f"Could not fetch prices for {co}. {e}")
        print(f"Could not fetch prices for {co}.")
    else:
        print(b)
    try:
        ohlc, last = kraken.get_ohlc(co, fi, interval=1, since=datetime.timestamp(datetime(year, month, day, 0, 0, 0)), ascending=True)
    except KrakenAPIError as e:
        print("KrakenAPIError:", e)
    except Exception as e:
        # print(f"Failed to retrieve OHLC data for {kraken.pairs[fi][co]}. {e}")
        print(f"Failed to retrieve OHLC data for {kraken.pairs[fi][co]}.")
    else:
        ohlc.set_index("time", inplace=True)
        # print(ohlc)
        print(last)
        pair = kraken.pairs[fi][co]
        path = f"{DIR_TMP}/{pair}.pkl"
        try: # If we have previous saved data, merge with the new data
            # TODO: choose which values to keep for equal timestamps covering different time intervals
            # Maybe keep them all, but in "parallel" dataframes? One for each time resolution.
            # And when fetching data, look for the closest timestamp with the smallest resolution for best accuracy.
            ohlc_old = pd.read_pickle(path)
            # ohlc_old.set_index("time", inplace=True)
            # ohlc_old.drop_duplicates()
        except FileNotFoundError:
            print(f"{path} does not exist. Creating...")
            pass
        else:
            print(f"{path} already exists. Loaded...")
            ohlc = ohlc.combine_first(ohlc_old)
        ohlc.to_pickle(path)
        print(ohlc)
        # import matplotlib.pyplot as plt
        # ohlc.plot(y="close", use_index=True, kind="line")
        # plt.show()

if __name__ == "__main__":
    test(2013, 1, 1)
