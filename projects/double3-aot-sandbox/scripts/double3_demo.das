options gen2

require double3_bindings

[export]
def run_entry(work_iters : int) : Double3 {
    let seed2 = add_double2(
        make_double2(double(1.0f), double(2.0f)),
        scale_double2(make_double2(double(0.25f), double(0.5f)), double(2.0f))
    )
    print("[das] double2 seed created\n")
    print_double2_from_cpp(seed2)

    let seed = lift_double2_to_double3(seed2, double(3.0f))
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
