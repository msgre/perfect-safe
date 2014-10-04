Perfect safe
============

![Perfect safe](https://raw.github.com/msgre/perfect-safe/master/img/trezor1.jpg)

Project built with littleBits modules and LEGO. There is a simple lock
mechanism connected to servo which holds door closed. When you set correct
passcode with dimmer, the door will be opened and lights inside the safe will
be turned on. After you close the door, lock mechanism secure safe again.

Watch this presentation video: http://youtu.be/yedk1cwqeQw

![Inside the safe](https://raw.github.com/msgre/perfect-safe/master/img/vnitrek.jpg)

Technically there is Arduino littleBits module with custom code.

![littleBits scheme](https://raw.github.com/msgre/perfect-safe/master/img/zapojeni.jpg)

Passcode is read from dimmer module connected to analog input. User just rotate
to correct position (number from scale 1..9) and wait for a while. There is
internal buffer filled every 200 ms with number sampled from analog port. When
sampled value is stable for 1.5 second, it is taken as one of the passcode
number, stored in RAM and Arduino beeps through buzzer module (to give user
feedback that number was taken).

After each given value passcode is compared to `PASSWORD` constant. If they are
same, servo is set to HIGH state and lock mechanism open the door. In the same
time, internal LED is turned on (because you want to see content of the safe).

There is also simple switch inside, so after you take content from safe and
close the door, switch is turn on and Arduino turn off the ligths and secure
lock with the help of servo.

Beware!
-------

Servo module sometimes does not play nice with Arduino and older batteries. If
you will have troubles with vibrations, change batterie with fresh one. If your
troubles remains, contact littleBits support please.