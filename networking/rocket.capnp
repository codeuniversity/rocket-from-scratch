interface Rocket {
    struct Data {
        time @0 :UInt32;
        gyro @1 :Gyro;
        acc  @2 :Acc;
        pressure @3 :Float64;
        temperature @4 :Float64;
        height @5 :Float64;
        est_alt_avg @6 :Float;

        struct Gyro {
            x @0 :Float64;
            y @1 :Float64;
            z @2 :Float64;
        }

        struct Acc {
            x @0 :Float64;
            y @1 :Float64;
            z @2 :Float64;
        }
    }

    send @0 (data :Data) -> ();
}

