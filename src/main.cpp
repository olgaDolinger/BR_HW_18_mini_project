#include <Arduino.h>

const int SENSOR_PIN = 6;  // Пін підключення дільника з фоторезистором (GPIO In)
const int RELAY_PIN = 4;   // Пін керування транзистором реле (GPIO Out)

const int THRESHOLD_DARK = 2200;
const int THRESHOLD_LIGHT = 2900;

const int NUM_READINGS = 10;
long int readings[NUM_READINGS];
int currentPosition = 0;

long int prevValue = 0;

void setup() {
    Serial.begin(115200);

    pinMode(RELAY_PIN, OUTPUT);

    digitalWrite(RELAY_PIN, LOW);

    // встановлення роздільної здатності АЦП (12 біт = 0...4095)
    analogReadResolution(12);
}

float getMovingAverage(long int* samples) {
    long sum = 0;
    for (int i = 0; i < NUM_READINGS; i++) {
        sum += samples[i];
    }
    return sum / NUM_READINGS;
}

// y = α · x + (1 − α) · y_prev
float applyFilter(long int input) {
    float alpha = 0.1;  // коефіцієнт фільтрації
    float filteredValue = alpha * input + (1 - alpha) * prevValue;
    prevValue = filteredValue;
    return filteredValue;
}

void loop() {
    // put your main code here, to run repeatedly:
    int sensorValue = analogRead(SENSOR_PIN);
    if (sensorValue < THRESHOLD_DARK) {
        digitalWrite(RELAY_PIN, HIGH);
    } else if (sensorValue > THRESHOLD_LIGHT) {
        digitalWrite(RELAY_PIN, LOW);
    }

    if (currentPosition < NUM_READINGS) {
        readings[currentPosition] = sensorValue;
        currentPosition++;
    } else {
        float movingAverage = getMovingAverage(readings);
        Serial.printf(">movingAverage:%f\n", movingAverage);
        currentPosition = 0;
    }

    float filteredValue = applyFilter(sensorValue);
    Serial.printf(">filtered:%f\n", filteredValue);

    // Формат Teleplot: ">назва:значення\n"
    Serial.printf(">sensor:%d\n", sensorValue);
    Serial.printf(">relay:%d\n", digitalRead(RELAY_PIN));

    delay(100);  // невелика затримка для стабільності читання сенсора
}