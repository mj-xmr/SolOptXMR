#!/bin/bash -e

tsqsim_deps() {
	cd externals/tsqsim/
	./util/deps-pull.sh
	cd ../..
}

system_deps() {
	if [ "$(uname)" == "Darwin" ]; then
		HOMEBREW_NO_AUTO_UPDATE=1 brew install rapidjson libffi osx-cpu-temp hdf5
		echo "Please enable the `at` scheduler manually:"
		echo "https://unix.stackexchange.com/questions/478823/making-at-work-on-macos/478840#478840"
	elif [ "$(uname)" == "Linux" ]; then
		sudo apt install gfortran libffi-dev python3-testresources python3.11-venv
		#sudo apt install libboost-all-dev # Only JSON is needed for now
		#sudo apt install libffi-dev
		sudo apt install rapidjson-dev lm-sensors fswebcam at
	fi
}


pip_reqs() {
	python3 -m venv .
	bin/python bin/pip install -r requirements.txt
}

python_deps() {
	if pip_reqs ; then
		echo "pip succeeded."
	else
		echo "pip failed. Trying a fallback."
		bin/python bin/pip uninstall -r requirements.txt -y
		bin/python bin/pip install pandas>=1.4.2 scipy>=1.2.0 numpy>=1.19.3 matplotlib \
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
}

tsqsim_deps
system_deps
python_deps



