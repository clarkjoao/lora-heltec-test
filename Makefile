
run_slave:
	@echo "Running slave"
	pio run -e slave -t upload
	sleep 3
	pio device monitor -b 115200 -e slave

run_master:
	@echo "Running master"
	pio run -e master -t upload
	sleep 3
	pio device monitor -b 115200 -e master

watch_slave:
	@echo "Watching slave"
	pio device monitor -b 115200 -e slave

watch_master:
	@echo "Watching master"
	pio device monitor -b 115200 -e master