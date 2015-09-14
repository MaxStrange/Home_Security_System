/**
This is the accumulator sketch. It is the node that is responsible for sounding the alarm if the right sequence of events
happen as reported by the other sensor nodes. To save money and space, it is also a mic/pir/switch sensor node.

It sounds the alarm if it receives a signal from two different nodes (including possibly itself). If it receives
a signal from a node, it enters the alert mode, whereupon it starts a countdown. If, within that countdown, it receives
a second signal (not from the same node), it enters intruder_detected mode and sounds the alarm. If it doesn't receive
the second signal within the alotted time, it resets back to all_clear mode.

If it enters intruder_detected mode, it will sound the alarm for several seconds, and wait for the disarm signal to come.
If it never gets the disarm signal, it eventually resets to all_clear.
*/
