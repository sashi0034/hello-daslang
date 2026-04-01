options gen2

let script_version = "v1"
var local_tick_count = 0
var accumulator = 0.0

[export]
def tick(dt : float; frame : int; uptime : float; reload_count : int) {
    local_tick_count += 23
    accumulator += dt

    print("[das] version={script_version}, local_tick_count={local_tick_count}, frame={frame}, uptime={uptime}, reload_count={reload_count}, accumulator={accumulator}\n")
}
