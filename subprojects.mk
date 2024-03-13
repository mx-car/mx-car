all: help

help:
	@echo ""
	@echo "   Help Menu"
	@echo ""
	@echo "   make clone            - clones the subrepostories needed"
	@echo ""
	
lib/car_bldc:
		git clone git@github.com:mx-car/car_bldc.git $@

lib/car_ros2:
		git clone git@github.com:mx-car/car_ros2.git $@
		
lib/car_com:
		git clone git@github.com:mx-car/car_com.git $@

lib/car_encoder:
		git clone git@github.com:mx-car/car_encoder.git $@

lib/car_math:
		git clone git@github.com:mx-car/car_math.git $@

lib/car_model:
		git clone git@github.com:mx-car/car_model.git $@

lib/car_time:
		git clone git@github.com:mx-car/car_time.git $@

lib/mx_objects:
		git clone git@github.com:mx-robotics/mx_objects.git $@

lib/mx_joystick:
		git clone git@github.com:mx-robotics/mx_joystick.git $@

clone:  \
	lib/car_bldc \
	lib/car_ros2 \
	lib/car_com \
	lib/car_encoder \
	lib/car_math \
	lib/car_model \
	lib/car_time \
	lib/mx_objects \
	lib/mx_joystick