# OCR module

## Intro
The OCR module allows to read the LCD displays, [like voltage](../src/data/img/lcd-glowing.jpg). 
The basic idea of this module is to allow you to be independent of any smart meters, doing the same readouts, should you have privacy concerns. 
Using the OCR module costs some additional initial effort, but allows your system to stay very flexible.

## Directory structure
The base project for the OCR module is located under `externals/GasPumpOCR-mj`. 
It's recommended to reuse the existing OCR modules from under `externals/GasPumpOCR-mj/custom-scripts` and modify them so that they can read your own LCD display, then of course saving them under a different directory.

Within the base directory there are 2 major scripts:
- `headless.py`, which performs the recognition based on an image file (`-f` param.) and a directory which contains a script (`-s` param.) with a file name `ocr_filter_module.py`. Any other name won't work.
- `playground.py`, which uses the same parameters to do the same job as `headless.py`, but additionally allows you to interactively alter the parameters of your OCR module to be able to see the final, as well as intermediate results immediately on your screen. After you find optimal paramerers, using this method, you're expected to store them in your module and update the global ones, like the example modules do in the `get_updated_params()` function.

## SolOptXMR integration
SolOptXMR uses only the `headless.py` script in order to read the battery's or inverter's voltage. It expects several things to be in place in `config-volatile.json`:
- `paths` / `DIR_TMP`, under which an image named `ocr.jpg` shall be read from, showing the LCD display. Obviously it's expected to be created there not too long before the recognition takes place, to have the result as fresh as possible. Otherwise a warning is issued.
- `paths` / `IMG_CAPTURE_PATH`, where the image should be stored after being captured. If the capture is meant to be done on the same host, the path should point to the file above, so (pseudocode:) `${DIR_TMP}/ocr.jpg`. If a different host is to be used, then the path here should point to a network drive accessible from that host. Next, you have to make sure, that the file is symlinked to `${DIR_TMP}/ocr.jpg` on the host, which controls the system.
- `paths` / `DIR_IMG_CAPTURE_SCRIPT`, you may create an individual script for capturing the image. See the provided examples.
- `hosts` / `IMG_CAPTURE_HOST`, which should stay either `localhost`, or the IP or alias of the machine, which has the camera connected.
- `paths` / `DIR_OCR_SCRIPT` variable pointing to a directory containing the `ocr_filter_module.py` file, that you selected to recognize your LCD or panel.

## Writing your own scripts
You may write your own scripts, having different fixed parameters and even different image processing logic. 
For this, copy one of the most fitting scripts of the known use cases from `externals/GasPumpOCR-mj/custom-scripts` and place it in another directory, keeping in mind, that the script's file name has to remain: `ocr_filter_module.py`. 
The script has to implement the following functions:
- `test()`: just a test function, to see if the import of your module succeeded. It makes sense to print there your module's individual name of your choice.
- `get_updated_params_image_processing(params_dict_ip)`: updating and returning the received dictionary with the image processing parameters of your choice, having them discovered via the `playground.py` script.
- `get_updated_params_image_recognition(params_dict_ir)`: updating and returning the received dictionary with the image recognition parameters, describing what shapes of countours should be taken as digits to recognize.

The parameters of the `get_updated_params_image_recognition(params_dict_ir)` include:
- `min_size_rectangle_one`: The minimum size of a rectangle, that represents digit `1`, which is an exception of its size and width to height relationship (a.k.a.: aspect ratio)
- `desired_aspect_digit_one`: The expected aspect ratio of the digit `1`
- `min_size_rectangle`: The minimum size of a rectangle, that represents a digit other than `1`, or any other rectangle of interest
- `desired_aspect_digit`: The expected aspect ratio of any digit other than `1`
- `final_number_multiplier`: Sometimes it's hard to recognize the decimal point, so setting this to a number any other than `1`, will allow to linearly transform the recognized number. For example `0.01` will suggest, that the recognized number should be divided by 100.
- `filter_width>height`: Setting this to `False` will allow you to recognize (read: not filter out) rectangles, whose width is larger than their height, so technically not digits.
- `countours_to_percentage_function`: Setting this a function pointer will make use of the pointer to convert the found countours to a percentage of the battery charge. See below.

# Simple mode: Rectangle recognition
There is a possibility to avoid digit recognition altogether, assuming, that your hardware is able to display rectangles, which represent the current battery charge, like [in this example](../src/data/img/panel-rectangles.jpg). 
An adaptation of the OCR module, that recognizes the mentioned image can be found under `externals/GasPumpOCR-mj/custom-scripts/panel-rectangles`.
Via the function `get_countours_to_percentage_full(num_countours)` such script should return a percentage in range from 0 to 100, describing the current battery charge, depending on the number of correctly recognized countours.
The recognition depends on the parameters, that the script sets via `get_updated_params_image_recognition()`.
As with the digit recognition module, the `playground.py` script will help you in finding the right parameters.

# Image capture module
In the same way as you'd extend the OCR modules from `externals/GasPumpOCR-mj/custom-scripts`, you may extend the SolOptXMR modules from `src/custom-scripts`. Image capture module is one of such examples. Finally, you have to point the `config-volatile.json` file to find your script via the `DIR_IMG_CAPTURE_SCRIPT` variable.

## Tips
Bear in mind, that the digits themselves have to take a large portion of the whole image. Photographing them from too far away will lead to not being able to recognize them.

