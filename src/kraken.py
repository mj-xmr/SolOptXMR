from enum import Enum, auto
import requests


class coin(Enum):
    XMR = auto()


class fiat(Enum):
    EUR = auto()
    USD = auto()


class kraken:
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


if __name__ == "__main__":
    a = kraken.get_price(coin.XMR, fiat.EUR)
    b = kraken.get_prices(coin.XMR)
    print(a)
    print(b)