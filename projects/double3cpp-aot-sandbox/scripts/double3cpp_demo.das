options gen2

require double3cpp_bindings

[export]
def run_entry(work_iters : int) : Double3 {
    let seed2 = Double2(double(1.0f), double(2.0f)).add(
        Double2(double(0.25f), double(0.5f)).scale(double(2.0f))
    )
    print("[das] double2 seed created\n")
    seed2.print()

    let seed = seed2.toDouble3(double(3.0f))
    print("[das] seed created\n")
    seed.print()

    var acc : Double3 = seed.transform()
    print("[das] after cpp_transform\n")
    acc.print()

    for (i in range(work_iters)) {
        let f = double(1.0f) + double((i % 13)) * double(0.000001f)
        acc = acc.scale(f).add(seed)
    }

    let out = acc.transform()
    print("[das] returning value\n")
    out.print()
    return out
}
