build: install_deps
	pebble build

run: build
	pebble install --emulator emery
	pebble install --emulator gabbro

runphone: build
	pebble install --phone "{{env_var('PHONE_IP')}}"

install_deps:
	npm install
	
compile_font: install_deps
	# Compile our font for all digits and all the letters in three  latter day names
	npx --no-install fctx-compiler fonts/BebasNeue-Regular.svg -r '[0-9:URSNIADHFTOWME]'
