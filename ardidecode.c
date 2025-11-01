#include <Arduino_LSM9DS1.h>
#include <MadgwickAHRS.h>

Madgwick filter;
unsigned long microsPerReading, microsPrevious;
float roll, pitch, yaw;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  filter.begin(100);   // 100 Hz filter update rate
  microsPerReading = 1000000 / 100;
  microsPrevious = micros();
}

void loop() {
  float gx, gy, gz;
  float ax, ay, az;
  float mx, my, mz;

  // Check if new data is available
  if (IMU.gyroscopeAvailable() && IMU.accelerationAvailable() && IMU.magneticFieldAvailable()) {
    IMU.readGyroscope(gx, gy, gz);
    IMU.readAcceleration(ax, ay, az);
    IMU.readMagneticField(mx, my, mz);

    unsigned long microsNow = micros();
    if (microsNow - microsPrevious >= microsPerReading) {
      filter.update(gx * DEG_TO_RAD, gy * DEG_TO_RAD, gz * DEG_TO_RAD, ax, ay, az, mx, my, mz);

      roll  = filter.getRoll();
      pitch = filter.getPitch();
      yaw   = filter.getYaw();

      // Send to Processing (tab-separated)
      Serial.print(roll);
      Serial.print("\t");
      Serial.print(pitch);
      Serial.print("\t");
      Serial.println(yaw);

      microsPrevious = microsNow;
    }
  }
}
