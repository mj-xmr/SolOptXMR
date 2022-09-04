#!/bin/bash -e

cd externals/tsqsim/
./util/deps-pull.sh
cd ../..

if [ "$(uname)" == "Darwin" ]; then
	HOMEBREW_NO_AUTO_UPDATE=1 brew install jsoncpp libffi osx-cpu-temp
elif [ "$(uname)" == "Linux" ]; then
	sudo apt install gfortran libffi-dev python3-testresources
	#sudo apt install libboost-all-dev # Only JSON is needed for now
	#sudo apt install libffi-dev
	sudo apt install libjsoncpp-dev lm-sensors fswebcam
fi

if pip3 install -r requirements.txt ; then
	echo "pip succeeded."
else
	echo "pip failed. Trying a fallback."
	pip3 uninstall -r requirements.txt -y
	pip3 install pandas>=1.4.2 scipy>=1.2.0 numpy>=1.19.3 matplotlib \
	pvlib pykrakenapi pyparsing pyrsistent python-dateutil \
	python-json-config pytz requests six urllib3 \
	beautifulsoup4 wget cairosvg Pillow>=9.1.1 geocoder \
	opencv-python screeninfo \
	# pytesseract not employed yet
	#pip3 install --upgrade 
	# Curses menu:
	#windows-curses==2.3.0 ; sys_platform == 'win32'
	# Ascii menu:
	#asciimatics==1.13.0
fi

