/**
This is the 4send node. It is The node responsible for sending
the arm/disarm signal to the rest of the nodes. It also
has sensors (PIR and magnetic switch) through a NOR
gate.

It sleeps until an interrupt on pin 2 wakes it, at which
point it finds out why by checking the software serial.
If the software serial is available, it is because there
is an RFID key waiting to be scanned. It scans the key
and checks it against known keys. If it matches, it
broadcasts the disarm signal. Otherwise it ignores it.

If it wakes up and the software serial is not waiting,
it checks the nRF to see if it received something.
If it received the disarm signal, it broadcasts the
disarm signal itself and then disarms and goes back to
sleep.

If it wakes up and finds none of the other reasons to be
why, it must be because the arm switch has been pushed.
It broadcasts the arm signal and arms itself.

Once armed, it sleeps and waits for interrupts on pin 2
as described above AND on pin 3, where it is waiting
for sensor input. If it receives an interrupt on pin 3,
it sends a threat signal to the accumulator node.
*/
