BUILD_DIR ?= build
GENERATOR ?= Ninja

.PHONY: configure build run clean

configure:
	cmake -S . -B $(BUILD_DIR) -G $(GENERATOR)

build:
	cmake --build $(BUILD_DIR)

run:
	./$(BUILD_DIR)/apps/Sandbox/nova_sandbox

clean:
	cmake --build $(BUILD_DIR) --target clean
