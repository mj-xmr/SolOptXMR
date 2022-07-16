# OCR module

## Intro
The OCR module allows to read the LCD displays, like voltage. The basic idea of this module is to allow you to be independent of any smart meters, doing the same readouts, should you have privacy concerns. 
Using the OCR module costs some additional initial effort, but allows your system to stay very flexible.

## Directory structure
The base project for the OCR module is located under `externals/GasPumpOCR-mj`. 
It's recommended to reuse the existing OCR modules from under `externals/GasPumpOCR-mj/custom-scripts` and modify them so that they can read your own LCD display, then of course saving them under a different directory.

Within the base directory there are 2 major scripts:
- `headless.py`, which performs the recognition based on an image file (`-f` param.) and a directory which contains a script (`-s` param.) with a file name `ocr_filter_module.py`. Any other name won't work.
- `playground.py`, which uses the same parameters to do the same job as `headless.py`, but additionally allows you to interactively alter the parameters of your OCR module to be able to see the final, as well as intermediate results immediately on your screen. After you find optimal paramerers, using this method, you're expected to store them in your module and update the global ones, like the example modules do in the `get_updated_params()` function.

## SolOptXMR integration
SolOptXMR uses only the `headless.py` script in order to read the battery's or inverter's voltage. It expects 2 things to be in place:
- `config-volatile.json` to have the `paths` / `DIR_TMP` variable defined, under which an image named `ocr.jpg` shall be created, showing the LCD display. Obviously it's expected to be created there not too long before the recognition takes place, to have the result as fresh as possible.
- `config-volatile.json` to have the `paths` / `DIR_OCR_SCRIPT` variable pointing to a directory containing the `ocr_filter_module.py` file, that you selected to recognize your LCD.

## Tips
Bear in mind, that the digits themselves have to take a large portion of the whole image. Photographing them from too far away will lead to not being able to recognize them.
