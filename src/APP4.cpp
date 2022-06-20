/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "d:/Transfer/Escuela/Universidad/S6/APP4/S6-APP4/src/APP4.ino"
#include "Particle.h"

// D2 = input
// D3 = output

void setup();
void loop();
void sendHIGH(void);
void sendLOW(void);
void risingInput(void);
void fallingInput(void);
double calculCLK();
#line 6 "d:/Transfer/Escuela/Universidad/S6/APP4/S6-APP4/src/APP4.ino"
SYSTEM_THREAD(ENABLED);

void threadInput(void);
void threadOutput(void);


//Thread thread("threadInput", threadInput);
//Thread thread("threadOutput", threadOutput);

int once = 0;
volatile int param = 3;
volatile int counter = 0;
volatile int input = 0;
unsigned long lastReport = 0;
//system_tick_t lastOuputThreadTime = 0;



//***//
void threadSendMessage(void);


//Write
volatile byte messagePaquet;
system_tick_t lastOuputThreadTime = 0;


//Read
volatile int up_counter = 0;
volatile int down_counter = 0;
volatile unsigned long clk_values[9] = {0};
volatile int clk_index = 0;
volatile unsigned long clk_time = 0;

//***//

void setup() {
	Serial.begin(9600);
	pinMode(D2, INPUT);
	pinMode(D3, OUTPUT);
	digitalWrite(D3, HIGH);
	attachInterrupt(D2, fallingInput, FALLING);
}

void loop() {
    
    delay(8000);
    
    //Create message
    byte message = 0xf0;  //1111 0000
    
    
    //Send message
    messagePaquet = message;
    new Thread("threadMessage", threadSendMessage);
    
    //Wait for message to be done 
    delay(10000);
    
    
    double clk = calculCLK();
    WITH_LOCK(Serial){
        Serial.printlnf("up_counter:%d",up_counter);
        Serial.printlnf("down_counter:%d",down_counter);
        Serial.printlnf("clock:%.2f", clk);
        Serial.printlnf("clock_index:%d", clk_index);
        Serial.printlnf("clock_time:%d", clk_time);
    }
    
    
	/*if (millis() - lastReport >= 12000) {
		lastReport = millis();
		
		
		if(once == 0){
		    new Thread("threadOutput", threadOutput);
		    once = 1;
		}
		
		int temp = analogRead(D2);

		WITH_LOCK(Serial) {
			//Serial.printlnf("counter=%d", counter);
			Serial.printlnf("read=%d", temp);
		}
	}*/
	
	//Reset output to basic
	digitalWrite(D3, HIGH);
	
	clk_index = 0;
	clk_time = 0;
	up_counter = 0;
	down_counter = 0;
}


void threadSendMessage(void) {
    WITH_LOCK(Serial){
        Serial.println("SENDING MESSAGE");
    }
    for(int i = 0; i < 8; i++){
        if(i%2 == 0){
            sendLOW();
        }
        else{
            sendHIGH();
        }
    }
    
    
    
    /*
    int bits[8] = {0};
    
    WITH_LOCK(Serial){
        for(int bit = 7; bit >= 0; --bit){
            bits[bit] = (message >> bit) & 1;
            Serial.printlnf("%d", bits[bit]);
            delay(200);
        }
        Serial.println("");
    }*/
}

void sendHIGH(void){ //1
    system_tick_t startOutputThread = millis();
    
    
    digitalWrite(D3, HIGH);
    //os_thread_delay_until(&startOutputThread, 1000);
    delay(10);
    digitalWrite(D3, LOW);
    delay(10);
    //os_thread_delay_until(&startOutputThread, 1000);
}

void sendLOW(void){ //0
    system_tick_t startOutputThread = millis();

    digitalWrite(D3, LOW);
    delay(10);
    //os_thread_delay_until(&startOutputThread, 1000);
    digitalWrite(D3, HIGH);
    delay(10);
    //os_thread_delay_until(&startOutputThread, 1000);
}


void risingInput(void){
    /*WITH_LOCK(Serial){
        Serial.println("RISING\n");
    }*/
    up_counter++;
    clk_values[clk_index] = millis();
    WITH_LOCK(Serial){
        Serial.printlnf("Temps: %d", millis());
    }
    clk_time += clk_values[clk_index];
    clk_index++;
    
    attachInterrupt(D2, fallingInput, FALLING);
}

void fallingInput(void){
    /*WITH_LOCK(Serial){
        Serial.println("FALLING");
    }*/
    down_counter++;
    clk_values[clk_index] = millis();
    WITH_LOCK(Serial){
        Serial.printlnf("Temps: %d", millis());
    }
    clk_time += clk_values[clk_index];
    clk_index++;
    
    attachInterrupt(D2, risingInput, RISING);
}


double calculCLK(){
    //if(clk_index == 9){
        unsigned long time1 = clk_values[2] - clk_values[1];
        unsigned long time2 = clk_values[4] - clk_values[3];
        unsigned long time3 = clk_values[6] - clk_values[5];
        unsigned long time4 = clk_values[8] - clk_values[7];
        
        WITH_LOCK(Serial){
            Serial.printlnf("clk0: %d", clk_values[0]);
            Serial.printlnf("clk1: %d", clk_values[1]);
            Serial.printlnf("clk2: %d", clk_values[2]);
            Serial.printlnf("clk3: %d", clk_values[3]);
            Serial.printlnf("clk4: %d", clk_values[3]);
            Serial.printlnf("clk5: %d", clk_values[5]);
            Serial.printlnf("clk6: %d", clk_values[6]);
            Serial.printlnf("clk7: %d", clk_values[7]);
            Serial.printlnf("clk8: %d", clk_values[8]);
            Serial.printlnf("Time1: %d", time1);
            Serial.printlnf("Time2: %d", time2);
            Serial.printlnf("Time3: %d", time3);
            Serial.printlnf("Time4: %d", time4);
        }
        
        double moy = ((double)time1+(double)time2+(double)time3+(double)time4)/4.0;
        
        return moy;
    //}
    //return 0;
}





//LEGACY CODE//

void threadInput(void) {
    int i = 0;
}

void threadOutput(void) {
    while(true){
        digitalWrite(D3, LOW);
        os_thread_delay_until(&lastOuputThreadTime, 1000);
        
        digitalWrite(D3, HIGH);
        os_thread_delay_until(&lastOuputThreadTime, 1000);
        
        WITH_LOCK(Serial){
            Serial.printlnf("param: %d", param);
        }
    }
    
}




/*void threadFunction(void) {
	while(true) {
		WITH_LOCK(Serial) {
			Serial.print(".");
		}
		counter++;

		// Delay so we're called every 100 milliseconds (10 times per second)
		os_thread_delay_until(&lastThreadTime, 100);
	}
	// You must not return from the thread function
}*/


