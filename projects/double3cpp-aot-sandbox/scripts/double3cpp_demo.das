options gen2

require double3cpp_bindings

[export]
def run_entry(work_iters : int) : Double3 {
    let base2 : Double2 = Double2(double(1.0f), double(2.0f))
    let offset2 : Double2 = Double2(double(0.25f), double(0.5f)).scale(double(2.0f))
    let seed2 : Double2 = base2 + offset2
    print("[das] double2 seed created\n")
    seed2.print()

    let seed : Double3 = seed2.toDouble3(double(3.0f))
    print("[das] seed created\n")
    seed.print()

    var acc : Double3 = seed.transform()
    print("[das] after cpp_transform\n")
    acc.print()

    for (i in range(work_iters)) {
        let f = double(1.0f) + double((i % 13)) * double(0.000001f)
        acc = acc.scale(f) + seed
    }

    let out : Double3 = acc.transform()
    print("[das] returning value\n")
    out.print()
    return out
}
