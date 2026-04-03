options gen2

require double3_demo

[export]
def run_entry(work_iters : int) : Double3 {
    let seed = make_double3(double(1.0f), double(2.0f), double(3.0f))
    print("[das] seed created\n")
    print_double3_from_cpp(seed)

    var acc : Double3 = cpp_transform(seed)
    print("[das] after cpp_transform\n")
    print_double3_from_cpp(acc)

    for (i in range(work_iters)) {
        let f = double(1.0f) + double((i % 13)) * double(0.000001f)
        acc = add_double3(scale_double3(acc, f), seed)
    }

    var out : Double3 = cpp_transform(acc)
    print("[das] returning value\n")
    print_double3_from_cpp(out)
    return out
}
