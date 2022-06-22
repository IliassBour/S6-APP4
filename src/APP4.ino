#include "Particle.h"
#include "FrameHandler.h"
#include "BitHandler.h"

// D2 = input
// D3 = output

SYSTEM_THREAD(ENABLED);

void threadInput(void);
void threadOutput(void);


//Thread thread("threadInput", threadInput);
//Thread thread("threadOutput", threadOutput);


//***//
//void threadSendMessage(void);


//Write//
//volatile byte messagePaquet[6];
system_tick_t lastOuputThreadTime = 0;


//Read//
//volatile int up_counter = 0;
//volatile int down_counter = 0;
//volatile unsigned long clk_values[10] = {0};
//volatile int clk_index = 0;
//volatile unsigned long clk_time = 0;

//***//

void setup() {
	Serial.begin(9600);
	pinMode(D2, INPUT);
	pinMode(D3, OUTPUT);
	digitalWrite(D3, HIGH);
	attachInterrupt(D2, BitHandler::fallingInterrupt, FALLING);
}

void loop() {
    /*byte trame[80] = {0};
    sendTrame("Hello world! Am I your friend?", trame);
    //sendWrongTrame(trame);
    receiveTrame(trame);
    
    for(int i =0; i <18; i++) {
        Serial.printf("trame %d: %x\n", i, trame[i]);
    }/**/

    delay(8000);
    
    //Create message
    //volatile byte BitHandler::messagePaquet[6] = {0x55, 0x7e, 0xf0, 0xff, 0x00, 0x7e};  //55(Clock) - 7e(Start/End) - f0(Entête/1111 0000) - ff(Data) - 00(CRC)
    //byte message = 0x15; //0001 0101

    //Send message
    //BitHandler::setMessage(message);
    new Thread("threadMessage", BitHandler::threadSendMessage);
    
    //Wait for message to be done
    //while(!BitHandler::read_end_flag){delay(10);} //À remplacer par une condition variable
    while (!BitHandler::end_flag)
    {
        delay(10);
    }

    byte received_msg[80] = {0x00};
    BitHandler::getRawMessageBytes(received_msg);  //byte[80]
    
    WITH_LOCK(Serial){
        Serial.println("FIN DE LA TRANSMISSION");
        for(int i = 0; i < sizeof(received_msg)/sizeof(received_msg[0]); i++){
            Serial.printf("%02x ", received_msg[i]);
        }
        Serial.print("\n");
    }
    
    /*double clk = BitHandler::calculCLK();
    WITH_LOCK(Serial){
        //Serial.printlnf("up_counter:%d",up_counter);
        //Serial.printlnf("down_counter:%d",down_counter);
        Serial.printlnf("clock:%.2f", clk);
        Serial.printlnf("clock_index:%d", BitHandler::clk_index);
        //Serial.printlnf("clock_time:%d", clk_time);
    }*/
	
	/*delay(1000);
	clk_time = 0;
	up_counter = 0;
	down_counter = 0;
    clk_index = 0;*/
}


//Double LEGACY CODE

/*void threadSendMessage(void) {
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
    
    int bits[8] = {0};
    
    WITH_LOCK(Serial){
        for(int bit = 7; bit >= 0; --bit){
            bits[bit] = (message >> bit) & 1;
            Serial.printlnf("%d", bits[bit]);
            delay(200);
        }
        Serial.println("");
    }
}*/

/*void sendHIGH(void){ //1
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
}*/

/*
void risingInput(void){
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

*/



//LEGACY CODE//
/*
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

*/


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


