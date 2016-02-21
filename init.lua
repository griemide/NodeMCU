-- init.lua
-- Michael Gries, 2016-02-09
-- Config
local pin = 4            --> GPIO2
local led = 0            --> GPIO16 onboard LED (no PWM support) 

local value = gpio.LOW
local duration = 1000    --> 1 second

-- Function toggles LED state
function toggleLED ()
    if value == gpio.LOW then
        value = gpio.HIGH
    else
        value = gpio.LOW
    end
    gpio.write(pin, value)
    gpio.write(led, value)
end

-- Initialise the pin
gpio.mode(pin, gpio.OUTPUT)
gpio.write(pin, value)

-- Initialise the onboard LED
gpio.mode(led, gpio.OUTPUT)
gpio.write(led, value)

-- Create an interval
tmr.alarm(0, duration, 1, toggleLED)
