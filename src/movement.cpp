#include "vex.h"
using namespace vex;
#include "math.h"
#include "screen_gui.hpp"
#include "helper_functions.hpp"
#include "movement.hpp"

#include <iostream>

using namespace vex;

int turninverse=-1;//change this to -1 if turning is inversed
//test
int JB;
int PB;
int PX;
int JX;

//General Sect;
//This section includes all general codes for drive and auto

void Zeroing(bool dist, bool HDG)
{
  if(dist){
    LF.resetPosition();
    LM.resetPosition();
    LB.resetPosition();
    RF.resetPosition();
    RM.resetPosition();
    RB.resetPosition();
  }
  if(HDG){
    Gyro.setHeading(0,degrees);
  }
}

ChassisDataSet ChassisUpdate()
{
  ChassisDataSet CDS;
  CDS.Left=get_dist_travelled((LF.position(degrees)+LM.position(degrees)+LB.position(degrees))/3.0);
  CDS.Right=get_dist_travelled((RF.position(degrees)+RM.position(degrees)+RB.position(degrees))/3.0);
  CDS.Avg=(CDS.Left+CDS.Right)/2;
  CDS.Diff=CDS.Left-CDS.Right;
  CDS.HDG=Gyro.heading(degrees);

  return CDS;
}

void Move(int left, int right)
{
  LF.setMaxTorque(100,percent);
  LM.setMaxTorque(100,percent);
  LB.setMaxTorque(100,percent);
  RF.setMaxTorque(100,percent);
  RM.setMaxTorque(100,percent);
  RB.setMaxTorque(100,percent);

  LF.spin(forward,(double)left/100.0*11,volt);
  LM.spin(forward,(double)left/100.0*11,volt);
  LB.spin(forward,(double)left/100.0*11,volt);
  RF.spin(forward,(double)right/100.0*11,volt);
  RM.spin(forward,(double)right/100.0*11,volt);
  RB.spin(forward,(double)right/100.0*11,volt);
}

void BStop()
{
  LF.setStopping(brake);
  LM.setStopping(brake);
  LB.setStopping(brake);
  RF.setStopping(brake);
  RM.setStopping(brake);
  RB.setStopping(brake);

  LF.stop();
  LM.stop();
  LB.stop();
  RF.stop();
  RM.stop();
  RB.stop();
}

void CStop()
{
  LF.setStopping(coast);
  LM.setStopping(coast);
  LB.setStopping(coast);
  RF.setStopping(coast);
  RM.setStopping(coast);
  RB.setStopping(coast);

  LF.stop();
  LM.stop();
  LB.stop();
  RF.stop();
  RM.stop();
  RB.stop();
}

void RunRoller(int val)
{
  IntakeB.setMaxTorque(100,percent);
  IntakeU.setMaxTorque(100,percent);
  IntakeB.spin(forward,(double)val/100.0*12,volt);
  IntakeU.spin(forward,(double)val/100.0*12,volt);
}
void RunrollerTop(int val)
{
  IntakeU.setMaxTorque(100,percent);
  IntakeU.spin(forward,(double)val/100.0*12,volt);
}

void RunrollerBottom(int val)
{
  IntakeB.setMaxTorque(100,percent);
  IntakeB.spin(forward,(double)val/100.0*12,volt);
}

int PrevE;//Error at t-1

void MoveEncoderPID(PIDDataSet KVals, int Speed, double dist,double AccT, double ABSHDG,bool brake){
  Speed *= -1;
  double CSpeed=0;
  Zeroing(true,false);
  ChassisDataSet SensorVals;
  SensorVals=ChassisUpdate();
  double PVal=0;
  double IVal=0;
  double DVal=0;
  double LGV=0;//define local gyro variable.
  PrevE=0;
  double Correction=0;
  Brain.Screen.clearScreen();

  while(fabs(SensorVals.Avg) <= fabs(dist))
  {
    if(fabs(CSpeed)<fabs((double)Speed))
    {
      CSpeed+=Speed/AccT*0.02;
    }

    SensorVals=ChassisUpdate();
    LGV=SensorVals.HDG-ABSHDG;
    if(LGV>180) LGV=LGV-360;
    PVal=KVals.kp*LGV;
    IVal=IVal+KVals.ki*LGV*0.02;
    DVal=KVals.kd*(LGV-PrevE);

    Correction=PVal+IVal+DVal/0.02;

    Move(CSpeed-Correction,CSpeed+Correction);
    PrevE=LGV;
    wait(20, msec);
  }
  if(brake){
    BStop();
    wait(120,msec);
  }
  else CStop();
}

void TurnMaxTimePID(PIDDataSet KVals,double DeltaAngle,double TE, bool brake){
  double CSpeed=0;
  Zeroing(true,false);
  ChassisDataSet SensorVals;
  SensorVals=ChassisUpdate();
  double PVal=0;
  double IVal=0;
  double DVal=0; 
  double LGV=0;
  PrevE=0;
  double Correction=0;
  Brain.Timer.reset();

  while(Brain.Timer.value() <= TE)
  {
    SensorVals=ChassisUpdate();
    LGV=SensorVals.HDG-DeltaAngle;
    if(LGV>180) LGV=LGV-360;
    PVal=KVals.kp*LGV;
    IVal=IVal+KVals.ki*LGV*0.02;
    DVal=KVals.kd*(LGV-PrevE);

    Correction=PVal+IVal+DVal/0.02;

    Move(CSpeed-Correction,CSpeed+Correction);
    PrevE=LGV;
    wait(20, msec);
  }
  if(brake){BStop();
    wait(180,msec);}
  else CStop();
}

void MaxTimePIDTurnOneSide(PIDDataSet KVals,double DeltaAngle,double TE, bool brake){
  double CSpeed=0;
  Zeroing(true,false);
  ChassisDataSet SensorVals;
  SensorVals=ChassisUpdate();
  double PVal=0;
  double IVal=0;
  double DVal=0;
  double LGV=0;
  PrevE=0;
  double Correction=0;
  double LV,RV;
  Brain.Timer.reset();

  while(Brain.Timer.value() <= TE)
  {
    SensorVals=ChassisUpdate();
    LGV=SensorVals.HDG-DeltaAngle;
    if(LGV>180) LGV=LGV-360;
    PVal=KVals.kp*LGV;
    IVal=IVal+KVals.ki*LGV*0.02;
    DVal=KVals.kd*(LGV-PrevE);

    Correction=PVal+IVal+DVal/0.02;
    LV=-CSpeed+Correction;
    RV=-CSpeed-Correction;
    if(LV>=0)LV=0;
    if(RV>=0)RV=0;
    Move(LV,RV);
    PrevE=LGV;
    wait(20, msec);
  }
  if(brake){BStop();
    wait(200,msec);}
  else CStop();
}

void MoveTimePID(PIDDataSet KVals, int Speed, double TE,double AccT,double ABSHDG, bool brake){
  double CSpeed=0;
  Zeroing(true,false);
  ChassisDataSet SensorVals;
  SensorVals=ChassisUpdate();
  double PVal=0;
  double IVal=0;
  double DVal=0;
  double LGV=0;
  PrevE=0;
  double Correction=0;
  Brain.Timer.reset();

  while(Brain.Timer.value() <= TE)
  {
    if(fabs(CSpeed)<fabs((double)Speed))
    {
      CSpeed+=Speed/AccT*0.02;
    }

    SensorVals=ChassisUpdate();
    LGV=SensorVals.HDG-ABSHDG;
    if(LGV>180) LGV=LGV-360;
    PVal=KVals.kp*LGV;
    IVal=IVal+KVals.ki*LGV*0.02;
    DVal=KVals.kd*(LGV-PrevE);

    Correction=PVal+IVal+DVal/0.02;

    Move(-CSpeed-Correction,-CSpeed+Correction);
    PrevE=LGV;
    wait(20, msec);
  }
  if(brake){BStop();
    wait(200,msec);}
  else CStop();
}


bool intakeDirection = false; // false = forward, true = reverse
bool lastL1 = false;



void SplitArcade() {
  int deadband = 5;

  int fwd = Controller1.Axis3.position();
  int turn = Controller1.Axis1.position();

  int leftPower = fwd + turn;
  int rightPower = fwd - turn;

  // Apply deadband
  if (abs(leftPower) < deadband) leftPower = 0;
  if (abs(rightPower) < deadband) rightPower = 0;

  LF.spin(forward, leftPower, pct);
  LM.spin(forward, leftPower, pct);
  LB.spin(forward, leftPower, pct);

  RF.spin(forward, rightPower, pct);
  RM.spin(forward, rightPower, pct);
  RB.spin(forward, rightPower, pct);
}