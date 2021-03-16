/**
 * @author Markus Bader <markus.bader@mx-robotics.com>
 */

#include <Arduino.h>
#include <vector>
#include <array>
#include <vector>
#include <numeric>
#include <limits> // std::numeric_limits
#include <algorithm>

#include <car/bldc/driver.h>
#include <car/math/pid.h>
#include <car/encoder/encoder_as5048a.h>
#include <car/math/angle.h>
#include <car/math/motion.h>
#include <car/time/cycle_rate.h>
#include <car/com/objects/text.h>
#include <car/com/mc/interface.h>

int SPI_SCK = 13;
int SPI_CS0 = 2;
int SPI_CS1 = 14;

car::encoder::Encoder *encoder;
car::bldc::Driver driver;
car::bldc::Motor motor0(std::array<uint8_t, 3>({33, 26, 31}),
                        std::array<uint8_t, 3>({10, 22, 23}),
                        std::array<uint8_t, 3>({A15, A16, A17}), 2, car::math::Direction::COUNTERCLOCKWISE);
car::bldc::Motor motor1(std::array<uint8_t, 3>({28, 8, 25}),
                        std::array<uint8_t, 3>({5, 6, 9}),
                        std::array<uint8_t, 3>({A15, A16, A17}), 14, car::math::Direction::COUNTERCLOCKWISE);
int loop_count = 0;
size_t delay_count = 0;
car::com::objects::Array16SC8 delays;

std::array<car::math::AngleDeg, 2> motor0_flux_offset({-65, -65 - 90});
std::array<car::math::AngleDeg, 2> motor1_flux_offset({-80 + 90, -80});
uint32_t timer_count;
car::time::CycleRate cycle_pwm(2);
car::time::CycleRate cycle_control(10);
car::time::CycleRate cycle_com(1000);          /// object for a constant cycle control

car::math::PID motor0_pid(0.1, -1, 1, 0.2, 0.05, 0.01);
car::math::PID motor1_pid(0.1, -1, 1, 0.2, 0.05, 0.01);


car::com::mc::Interface msg_tx;   				/// object to hande the serial communication
car::com::mc::Interface msg_rx;   				/// object to hande the serial communication
car::time::CycleRate cycle_rate(1000);    /// object for a constant cycle control
car::com::objects::Text text;             /// object to send
car::com::objects::AckermannState *ackermann_state = NULL;              
car::com::objects::AckermannConfig *ackermann_config = NULL;             

// the setup routine runs once when you press reset:
void setup()
{
  car::math::AngleDeg::init();
  driver.init(&motor0, &motor1);
  encoder = new car::encoder::AS5048A(std::array<uint8_t, 2>({motor0.pin_encoder_cs(), motor1.pin_encoder_cs()}), SPI_SCK);

  Serial.begin(115200); /// init serial
  while (!Serial);
  msg_rx.try_sync();   			    /// blocks until a sync message arrives

  motor0.init(11, motor0_flux_offset, std::bind(&car::encoder::Encoder::read, encoder, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
              std::bind(&car::bldc::Driver::update_pwm, &driver, std::placeholders::_1, std::placeholders::_2),
              std::bind(&car::bldc::Driver::couple_pwm, &driver, std::placeholders::_1, std::placeholders::_2));
  motor0.couple(true);
  motor0.set_power(0);

  motor1.init(11, motor1_flux_offset,
              std::bind(&car::encoder::Encoder::read, encoder, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
              std::bind(&car::bldc::Driver::update_pwm, &driver, std::placeholders::_1, std::placeholders::_2),
              std::bind(&car::bldc::Driver::couple_pwm, &driver, std::placeholders::_1, std::placeholders::_2));
  motor1.couple(true);
  motor1.set_power(0);
    delays.clear();
}
// the loop routine runs over and over again forever:
void loop()
{
  if(delay_count >= delays.size()) {
    delay_count = 0;
    delays.clear();
  }
  delays[delay_count++] = cycle_pwm.wait();
  {
    motor0.update_pwm();
    motor1.update_pwm();
  }
  if (cycle_control.passed())
  {
    motor0.update_control();
    motor1.update_control();

    if(ackermann_state != NULL){
      motor0.set_power(motor0_pid.update(ackermann_state->v[ackermann_state->LEFT], motor0.rps()));
      motor1.set_power(motor1_pid.update(ackermann_state->v[ackermann_state->RIGHT], motor1.rps()));
    }
  }
  if(cycle_com.passed() > 0){
      using namespace car::com::objects;
        msg_tx.reset();				/// removes all objects in message
        if(!text.empty()) {
          msg_tx.push_object ( Object (text, TYPE_TEXT ) );
        }
        if(ackermann_state != NULL){
           msg_tx.push_object(Object(*ackermann_state, TYPE_ACKERMANN_STATE));
        }
        msg_tx.push_object(Object(delays, TYPE_ARRAY16SC8));
        
        msg_tx.send();				        /// sends the message
    }
    if ( msg_rx.receive() ) {			/// check for messages
        using namespace car::com::objects;
        static Object object;
        while ( msg_rx.pop_object ( object ).isValid() ) {
            switch ( object.type ) {
            case TYPE_SYNC: 	/// case sync object
                Time::compute_offset ( msg_rx.stamp ); /// set clock
                break;
            case TYPE_ACKERMANN_CONFIG: 	/// case sync object
                if(ackermann_config == NULL) ackermann_config = new car::com::objects::AckermannConfig;
                object.get(*ackermann_config);
                break;
            case TYPE_ACKERMANN_STATE: 	/// case sync object
                if(ackermann_state == NULL) ackermann_state = new car::com::objects::AckermannState;
                object.get(*ackermann_state);
                break;
            default:/// case unkown type
                text.write ( "Unknown type received" );
                continue;
            }
        }
    }
  loop_count++;
}