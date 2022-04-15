from pykrakenapi import KrakenAPI
from datetime import datetime
from enum import Enum, auto
import krakenex
import requests


class coin(Enum):
    XMR = auto()


class fiat(Enum):
    EUR = auto()
    USD = auto()


class kraken:
    api = krakenex.API()
    k = KrakenAPI(api)
    pairs = {
        fiat.EUR: {
            coin.XMR: "XXMRZEUR",
        },
        fiat.USD: {
            coin.XMR: "XXMRZUSD",
        }
    }

    @staticmethod
    def get_price(co: coin, fi: fiat):
        pair = kraken.pairs[fi][co]
        data = {"pair": pair}
        coin_ticker = requests.post("https://api.kraken.com/0/public/Ticker", data=data).json()
        coin_price = float(coin_ticker["result"][pair]["c"][0])
        return coin_price

    @staticmethod
    def get_prices(co: coin):
        prices = {}
        for fi in fiat:
            pair = kraken.pairs[fi][co]
            data = {"pair": pair}
            coin_ticker = requests.post("https://api.kraken.com/0/public/Ticker", data=data).json()
            prices[fi] = float(coin_ticker["result"][pair]["c"][0])
        return prices

    @staticmethod
    def get_ohlc(co:coin, fi:fiat, interval=15, since=None, ascending=True):
        pair = kraken.pairs[fi][co]
        ohlc, last = kraken.k.get_ohlc_data(pair, interval=interval, since=since, ascending=ascending)
        return ohlc, last


if __name__ == "__main__":
    import pandas as pd
    fi = fiat.EUR
    co = coin.XMR
    a = kraken.get_price(co, fi)
    b = kraken.get_prices(co)
    ohlc, last = kraken.get_ohlc(co, fi, interval=15, since=datetime.timestamp(datetime(2013, 1, 1, 0, 0, 0)), ascending=True)
    ohlc.set_index("time")
    print(a)
    print(b)
    # print(ohlc)
    print(last)
    pair = kraken.pairs[fi][co]
    path = f"tmp/{pair}.pkl"
    try: # If we have previous saved data, merge with the new data
        # TODO: choose which values to keep for equal timestamps covering different time intervals
        # Maybe keep them all, but in "parallel" dataframes? One for each time resolution.
        # And when fetching data, look for the closest timestamp with the smallest resolution for best accuracy.
        ohlc_old = pd.read_pickle(path)
    except FileNotFoundError:
        print(f"{path} does not exist. Creating...")
        pass
    else:
        print(f"{path} already exists. Loaded...")
        ohlc = pd.merge_ordered(ohlc, ohlc_old, how="outer")
    ohlc.to_pickle(path)
    print(ohlc)
