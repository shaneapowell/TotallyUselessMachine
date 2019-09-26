/************************************************
 * 
 * Power Consumption Metrics:
 *  Stock:             20mA
 *  Power LED removed: 14mA
 *  On Board LED On:   22mA
 *  Sleep Mode OnBoard off: 000mA
 *  Sleep Mode OnBoard On:  7mA
 ************************************************/

#include <Arduino.h>
#include <PWMServo.h>
#include <LowPower.h>

#define ARM_PIN  		9
#define DOOR_PIN  		10
#define TRIGGER_PIN 	2
#define RELAY_PIN 		11

#define ARM_HOME  120
#define ARM_FULL  8

#define DOOR_CLOSED 150
#define DOOR_OPEN 60

#define SERVO_FAST  	1
#define SERVO_SLOW  	70

PWMServo doorServo;
PWMServo armServo;



/************************************************
 * Move the provided servo from position to position, with 
 * a n microseconds delay between each step.
 ************************************************/
void servoFromTo(PWMServo& servo, int from, int to, unsigned int delayms)
{
	if (delayms == 0)
	{
		servo.write(to);
		return;
	}

	int direction = from < to ? 1 : -1;	
	while (from != to)
	{
		servo.write(from);
		delay(delayms);
		from += direction;
	}

}

/************************************************
 * Move the provided servo from it's current position,
 * to position, with a n microseconds delay between each step.
 ************************************************/
void servoTo(PWMServo& servo, int to, unsigned int delayms)
{
	servoFromTo(servo, servo.read(), to, delayms);
}

/************************************************
 * Move the door to postion "to" at "speed".
 * to = 0 - 100
 * speed = 0 - 100
 * After the move, wait n milliseconds
 ************************************************/
void doorTo(int to, int speed, int wait)
{
	unsigned int iTo = map(to, 0, 100, DOOR_CLOSED, DOOR_OPEN);
	unsigned int iSpeed = map(abs(speed - 100), 0, 100, SERVO_FAST, SERVO_SLOW);
	servoTo(doorServo, iTo, speed < 0 ? 0 : iSpeed);
	delay(wait);
}

/************************************************
 * Move the arm to position "to" at "speed"
 * to = 0 - 100 
 * speed = 0 - 100
 * After the move, wait n milliseconds.
 * It takes 500ms for the arm to move full sweep.
 ************************************************/
void armTo(int to, int speed, int wait)
{
	unsigned int iTo = map(to, 0, 100, ARM_HOME, ARM_FULL);
	unsigned int iSpeed = map(abs(speed - 100), 0, 100, SERVO_FAST, SERVO_SLOW);
	servoTo(armServo, iTo, speed < 0 ? 0 : iSpeed);
	delay(wait);
}


/************************************************
 * Ready the servos for control, including turning on the
 * power relay.
 ************************************************/
void readyServos()
{
	pinMode(RELAY_PIN, OUTPUT);
	digitalWrite(RELAY_PIN, HIGH);
	delay(50);

	doorServo.write(DOOR_CLOSED);
	armServo.write(ARM_HOME);

	doorServo.attach(DOOR_PIN);
	armServo.attach(ARM_PIN);

	doorServo.write(DOOR_CLOSED);
	armServo.write(ARM_HOME);

}

/************************************************
 * Park the servos back to the start, turn off the
 * power relay.
 ************************************************/
void parkServos()
{
	doorServo.write(DOOR_CLOSED);
	armServo.write(ARM_HOME);
	
	delay(1000);
	digitalWrite(RELAY_PIN, LOW);

	doorServo.detach();
	armServo.detach();
}


// Sequences from "Moody Useless Machine" by Lamja Electronics
// http://www.lamja.com/?p=451
// http://www.lamja.com/blogfiles/UselessMachine.pde

/************************************************/
void sequence1()
{
	doorTo(40,  60,   1000);
	doorTo(0,   100, 1000);
	doorTo(100, 100,  50);
	 armTo(100, 100, 300);
	 armTo(0,   100, 100);
	doorTo(0,   100, 400);
}

/************************************************/
void sequence2()
{
	for (int i = 0; i < 2; i++)
	{
		doorTo(40, 80, 500);
		
		doorTo(00, -1, 150);
		doorTo(15, -1, 150);
		doorTo(00, -1, 150);
		doorTo(15, -1, 150);
		doorTo(00, -1, 150);
		doorTo(15, -1, 150);
		doorTo(00, -1, 150);
		doorTo(15, -1, 150);
		doorTo(00, -1, 150);
	}
	
	doorTo(40, 80, 1500);
	doorTo(100, 100, 100);
	
	armTo(100, 100, 400);
	armTo(0, 100, 200);
	doorTo(0, 100, 0);
}

/************************************************/
void sequence3()
{
	doorTo(100, 100, 10);
	armTo(100, 100, 400);
	armTo(0, 100, 10);
	doorTo(0, 100, 10);
}

/************************************************/
void sequence4()
{
	doorTo(100, 100, 0);
	armTo(70, 100, 500);
	armTo(100, 0, 1000);
	armTo(0, 100, 200);
	doorTo(0, 100, 200);
}

/************************************************/
void sequence5()
{
	doorTo(100, 100, 200);
	armTo(100, 100, 500);
	armTo(70, 100, 200);
	armTo(100, 100, 400);
	armTo(70, 100, 200);
	armTo(100, 100, 400);
	armTo(70, 100, 500);
	armTo(100, 100, 600);
	armTo(0, 100, 300);
	doorTo(0, 100, 400);
}

/************************************************/
void sequence6()
{
	doorTo(100, 100, 400);
	armTo(100, 100, 500);
	armTo(0, 100, 500);
	doorTo(20, 60, 1000);
	doorTo(70, 90, 500);
	doorTo(20, 30, 1000);
	doorTo(70, 100, 500);
	doorTo(40, 30, 500);
	doorTo(0,  50, 100);
}

/************************************************/
void sequence7()
{
	doorTo(50, 100, 500);
	doorTo(10, 50, 10);
	doorTo(50, 100, 800);
	doorTo(10, 50, 300);
	doorTo(50, 100, 1500);
	doorTo(10, 50, 500);
	doorTo(0, 100, 1000);
	doorTo(100, 100, 10);
	armTo(100, 100, 400);
	armTo(0, 100, 10);
	doorTo(0, 100, 500);
}

/************************************************/
void sequence8()
{
	doorTo(50, 100, 400);
	doorTo(65, 100, 50);
	doorTo(40, 100, 50);
	doorTo(65, 100, 50);
	doorTo(40, 100, 50);
	doorTo(65, 100, 50);
	doorTo(40, 100, 50);
	doorTo(65, 100, 50);
	doorTo(40, 100, 50);
	doorTo(65, 100, 50);
	doorTo(40, 100, 50);
	doorTo(65, 100, 50);
	doorTo(40, 100, 50);
	doorTo(65, 100, 50);
	doorTo(40, 100, 50);
	doorTo(65, 100, 50);
	doorTo(40, 100, 50);
	doorTo(0, 100, 200);
	doorTo(100, 100, 300);
	armTo(100, 100, 400);
	armTo(0, 100, 400);
	doorTo(0, 100, 500);
}

/************************************************/
void sequence9()
{
	doorTo(20, 100, 500);
	doorTo(40, 100, 100);
	doorTo(65, 100, 50);
	doorTo(40, 100, 50);
	doorTo(65, 100, 50);
	doorTo(40, 100, 50);
	doorTo(65, 100, 50);
	doorTo(40, 100, 50);
	doorTo(65, 100, 50);
	doorTo(40, 100, 50);
	doorTo(65, 100, 50);
	doorTo(40, 100, 50);
	doorTo(65, 100, 50);
	doorTo(40, 100, 50);
	doorTo(65, 100, 50);
	doorTo(40, 100, 50);
	doorTo(65, 100, 50);
	doorTo(40, 100, 500);
	doorTo(100, 100, 1);
	armTo(80, 100, 500);
	armTo(100, 100, 100);
	armTo(80, 100, 500);
	armTo(0, 100, 500);
	doorTo(0, 100, 500);
}

/************************************************/
void sequence10()
{
	doorTo(10, 100, 1);
	doorTo(100, 30, 1);
	armTo(100, 30, 1);
	armTo(30, 30, 1);
	armTo(0, 100, 1);
	doorTo(0, 100, 1);
}

/************************************************/
void sequence11()
{
	doorTo(100, 100, 200);
	armTo(100, 100, 200);
	armTo(80, 80, 1000);
	armTo(40, 80, 1000);
	doorTo(40, 60, 10);
	doorTo(100, 100, 100);
	armTo(100, 100, 2000);
	armTo(30, 60, 1000);
	doorTo(40, 40, 10);
	doorTo(100, 100, 100);
	armTo(100, 100, 2000);
	armTo(60, 80, 500);
	armTo(100, 100, 2000);
	armTo(0, 60, 100);
	doorTo(0, 40, 100);

}


/************************************************/
void sequence12()
{
	doorTo(100, 100, 1);
	armTo(100, 100, 10000);
	armTo(0, 100, 1);
	doorTo(0, 100, 1);
}


/************************************************/
void sequence13()
{
	doorTo(100, 100, 1);
	armTo(100, 100, 200);
	armTo(80, 100, 150);
	armTo(100, 100, 150);
	armTo(80, 100, 150);
	armTo(100, 100, 150);
	armTo(80, 100, 150);
	armTo(100, 100, 150);
	armTo(80, 100, 150);
	armTo(100, 100, 150);
	armTo(0, 100, 150);
	doorTo(0, 100, 1);
}


/************************************************/
void sequence14()
{
	doorTo(100, 100, 1);
	armTo(80, 100, 200);
	armTo(100, 0, 30);
	armTo(0, 100, 100);
	doorTo(0, 100, 1);
}

/************************************************
 * ISR
 ************************************************/
void activateTriggerInterrupt()
{
	/* Do nothing, we just need the interrupt to release the sleep mode */
}

/************************************************
 * 
 ************************************************/
void setup()
{
	Serial.begin(115200);

	digitalWrite(LED_BUILTIN, HIGH);
	pinMode(TRIGGER_PIN, INPUT_PULLUP);

	attachInterrupt(digitalPinToInterrupt(TRIGGER_PIN), activateTriggerInterrupt, FALLING);

	readyServos();
	parkServos();

	/* Double flash to show a normal boot */
	digitalWrite(LED_BUILTIN, HIGH);
	delay(200);
	digitalWrite(LED_BUILTIN, LOW);
	delay(200);
	digitalWrite(LED_BUILTIN, HIGH);
	delay(200);

	randomSeed(analogRead(0));
	
}


/* An array of all possible sequence functions */
void (*sequenceFPtr[])(void) = 
{
	sequence1, 
	sequence2, 
	sequence3, 
	sequence4,
	sequence5, 
	sequence6,
	sequence7,
	sequence8,
	sequence9,
	sequence10,
	sequence11,
	sequence12,
	sequence13,
	sequence14
};

/************************************************
 * The loop is blocked until the pin2 interrupt unblocks it.
 ************************************************/
void loop()
{

	/* Block on low-power mode here.  When we wake back up .. fire off a sequence 
	 But, if we got here, and the trigger pin is still low, someone turned it back on already */
	if (digitalRead(TRIGGER_PIN) == HIGH)
	{
		digitalWrite(LED_BUILTIN, LOW);
		LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
	}

	digitalWrite(LED_BUILTIN, HIGH);

	/* Random Sequence */
	readyServos();

	//  sequence14();
	
	int sequenceId = random(0, sizeof(sequenceFPtr) / sizeof(sequenceFPtr[0]));
	Serial.print("SID: "); Serial.println(sequenceId);
	sequenceFPtr[sequenceId]();

	parkServos();

}


