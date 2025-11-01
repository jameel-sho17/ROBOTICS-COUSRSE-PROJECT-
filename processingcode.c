import processing.serial.*;
import processing.opengl.*;

Serial port;                         

float roll = 0, pitch = 0, yaw = 0;

void setup() {
    // 600px viewport using OpenGL rendering
    size(600, 600, OPENGL);

    // setup lights and antialiasing
    lights();
    smooth();
  
    // display serial port list for debugging
    println(Serial.list());

    // pick your Arduino COM port (adjust index if needed!)
    String portName = Serial.list()[0];   // or "COM7" on Windows
    port = new Serial(this, portName, 115200);
    port.bufferUntil('\n');   // read full lines
}

void draw() {
    background(0);
    
    // translate everything to the middle of the viewport
    pushMatrix();
    translate(width / 2, height / 2);

    // apply rotations (convert degrees to radians)
    rotateX(radians(pitch));
    rotateY(radians(yaw));
    rotateZ(radians(roll));

    // draw main body in red
    fill(255, 0, 0, 200);
    box(50, 20, 200);
    
    // draw front-facing tip in blue
    fill(0, 0, 255, 200);
    pushMatrix();
    translate(0, 0, -120);
    rotateX(PI/2);
    drawCylinder(0, 20, 40, 16);
    popMatrix();
    
    // draw wings and tail fin in green
    fill(0, 255, 0, 200);
    beginShape(TRIANGLES);
    vertex(-120,  5, 30); vertex(0,  5, -80); vertex(120,  5, 30);  // wing top
    vertex(-120, -5, 30); vertex(0, -5, -80); vertex(120, -5, 30);  // wing bottom
    endShape();
    
    popMatrix();
}

void serialEvent(Serial port) {
    String inData = port.readStringUntil('\n');  // read line
    if (inData != null) {
        inData = trim(inData);
        String[] values = split(inData, '\t');   // Arduino sends tab-separated values
        if (values.length == 3) {
            roll = float(values[0]);
            pitch = float(values[1]);
            yaw = float(values[2]);
        }
    }
}

void drawCylinder(float topRadius, float bottomRadius, float tall, int sides) {
    float angle = 0;
    float angleIncrement = TWO_PI / sides;
    beginShape(QUAD_STRIP);
    for (int i = 0; i < sides + 1; ++i) {
        vertex(topRadius*cos(angle), 0, topRadius*sin(angle));
        vertex(bottomRadius*cos(angle), tall, bottomRadius*sin(angle));
        angle += angleIncrement;
    }
    endShape();
    
    if (topRadius != 0) {
        angle = 0;
        beginShape(TRIANGLE_FAN);
        vertex(0, 0, 0);
        for (int i = 0; i < sides + 1; i++) {
            vertex(topRadius * cos(angle), 0, topRadius * sin(angle));
            angle += angleIncrement;
        }
        endShape();
    }
  
    if (bottomRadius != 0) {
        angle = 0;
        beginShape(TRIANGLE_FAN);
        vertex(0, tall, 0);
        for (int i = 0; i < sides + 1; i++) {
            vertex(bottomRadius * cos(angle), tall, bottomRadius * sin(angle));
            angle += angleIncrement;
        }
        endShape();
    }
}
