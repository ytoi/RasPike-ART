
python -m venv ../tools/python
../tools/python/bin/pip install pyusb
sudo apt install minicom
echo "SUBSYSTEM==\"tty\", ATTRS{idVendor}==\"0483\",ATTRS{idProduct}==\"5740\", SYMLINK+=\"USB_SPIKE\",MODE=\"0666\"" > /tmp/serial_temp_rule
sudo mv /tmp/serial_temp_rule /etc/udev/rules.d/99-serial.rules
sudo udevadm control --reload
