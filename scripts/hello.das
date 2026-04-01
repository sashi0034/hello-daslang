options gen2

[export]
def hello_from_cpp(times : int; tag : string) {
    print("hello_from_cpp called: times={times}, tag='{tag}'\n")
}

[export]
def make_message(name : string) : string {
    return "Hello, {name}, from daScript"
}
