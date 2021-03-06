#!/bin/bash -e

cd externals/tsqsim/
./util/deps-pull.sh
cd ../..

if [ "$(uname)" == "Darwin" ]; then
	HOMEBREW_NO_AUTO_UPDATE=1 brew install rapidjson libffi osx-cpu-temp
elif [ "$(uname)" == "Linux" ]; then
	sudo apt install gfortran libffi-dev
	#sudo apt install libboost-all-dev # Only JSON is needed for now
	#sudo apt install libffi-dev
	sudo apt install rapidjson-dev lm-sensors fswebcam
fi

if pip3 install -r requirements.txt ; then
	echo "pip succeeded."
else
	echo "pip failed. Trying a fallback."

	pip3 install pandas>=1.4.2 pvlib pykrakenapi pyparsing pyrsistent python-dateutil \
	python-json-config pytz requests scipy six urllib3 \
	beautifulsoup4 wget cairosvg Pillow geocoder \
	opencv-python screeninfo \
	# pytesseract not employed yet
	#pip3 install --upgrade 
	# Curses menu:
	#windows-curses==2.3.0 ; sys_platform == 'win32'
	# Ascii menu:
	#asciimatics==1.13.0
fi

