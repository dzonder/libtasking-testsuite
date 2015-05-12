target remote localhost:2331

monitor SWO Start 0 1000000

set confirm off
set breakpoint pending on
set pagination off
set print pretty on

define timestamp
	call time_get()
end

define rst
	monitor reset
	monitor reset
	c
end

define lrst
	load
	rst
end
