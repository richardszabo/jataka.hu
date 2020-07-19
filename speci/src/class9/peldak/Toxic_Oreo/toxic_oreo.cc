//***************************************************************************
//
//	Keith Wiley
//  Copyright, 1999.  All rights reserverd.
//	Webots driver for the ECAL'99 contest
//	submitted version X
//
//***************************************************************************

#include <Khepera.h>
#include <KheperaK6300.h>
#include <gui.h>
#include <math.h>

//  GLOBAL DEFINES  *******************************************************
//filter object-type codes.
#define BLANK					0
#define SKY						1
#define GROUND					2
#define FEEDERBOX				3
#define FEEDER_OFF				4
#define FEEDER_ON				5
#define SIDEWALL				6

#define WHITEBOX				8
#define MAGENTABOX				9
#define YELLOWBOX				10
#define CYANBOX					11
#define DISK					12
#define RED_CORNER				13
#define CYAN_CORNER				14

//color are defined in a 3D array.  These define the elements pertaining to the three dimensions.
#define RED						0
#define GREEN					1
#define BLUE					2

//red, green, or blue, must be within this range of a targeted value to be recognized as equal to that value for filtering purposes.
#define COLOR_TOLERANCE			15

//used to define whether a feeder is visible from a particular landmark.  used in the WorldKnowledge data structure.
#define FEEDER_VISIBLE_UNKNOWN	-1
#define FEEDER_NOT_VISIBLE		0
#define FEEDER_IS_VISIBLE		1

#define NUMBEHAVIORS				14
#define MAXIMMEDIATELANDMARKS		10	
#define MAXNUMMEMORIZEDLANDMARKS	10

struct Box
{
	int16 top;
	int16 left;
	int16 bottom;
	int16 right;
	
	int16 width;
	int16 height;
	
	int16 horizontalCenter;
};

struct WorldKnowledge
{
	int16 feederDistances[7];							//feeder distances from each landmark.
	int16 landmarkSeparationDistances[7][7];			//landmark to landmark distances.
	int16 landmarkSeparationAngles[7][7][7];			//landmark to landmark angles when viewed from a third landmark.

	uint32 timeSinceLastGoal[7];						//TimeSteps since each landmark was a goal.
	bool needsAssociating[7];							//of the different landmark ids, which ones don't have associations yet.
	int16 feederVisible[7];								//for each landmark id, can a feeder be seen from that landmark.
};

struct ImmediateViewKnowledge
{
	int16 numImmediateLandmarks;						//number of landmarks immediately visible.
	int16 visibleLandmarks[MAXIMMEDIATELANDMARKS];		//id type of visible landmarks.
	Box landmarkBoxes[MAXIMMEDIATELANDMARKS];			//bounding boxes around visible landmarks.
	int8 landmarkDistances[MAXIMMEDIATELANDMARKS];		//distances to landmarks from present position.
};

struct PresentTimeStepData
{
	int16 presentLandmark;								//which landmark if is the robot presently located at.
	bool presentLandmarkOutOfView;						//is the present landmark not visible (because the robot turned away)?
	int16 presentLandmarkMemoryIndex;					//index of the present landmark in memory.
	int16 presentLandmarkGoal;							//id of the present landmark goal.
	int16 presentLandmarkGoalMemoryIndex;				//index of the landmark goal in memory.
	Box presentLandmarkGoalBox;							//bounding box around present landmark goal.
};

//	FUNCTION PROTOTYPES, in order within the file  *************************
int16 Random(int16 range);
void InitializeData();
void Create_camera_image();
void Paint_black_camera_image();
void Paint_black_filter_image();
void Gui_callback();
void Create_gui();
void Change_camera_image();
void Update_filter_camera();
void CreateFilteredImages();
bool CheckFeederAgainstAnotherKhepera();					//Checks to see if a patch of green is a feeder or another khepera robot.
int16 LookForFeederHorizCenter();
int16 LookForLandmarkHorizCenter();
void SetNeedToTryAgain(int16 direction);					//midway through a glance (at 90 degrees), check for a wall dead ahead.  If so, glance will try again.
void IdentifyImmediateLandmarks();							//takes the filtered view and creates a data structure of the presently visible landmarks.
int8 LookUpDistance(int8 objectType, Box boundBox);			//returns the distance to an object based on a lookup table using the object type and attributes of its bounding box.
void UpdateLandmarkDistanceRelationships(int8 objectType);	//updates world knowledge of the distance between landmarks.
bool FindWallOnLeft();										//used by glanceLeft.
bool FindWallOnRight();										//used by glanceRight.

//  GLOBAL VARIABLES  *******************************************************
const int16 k6300_halfWidth = K6300_WIDTH / 2;
const int16 k6300_widthMinus1 = K6300_WIDTH - 1;
const int16 k6300_heightMinus1 = K6300_HEIGHT - 1;
gui_window theGuiWindow;								//the gui window.
gui_image cameraImage = NULL;							//raw camera image.
gui_image filterImage = NULL;							//filtered image with each object-type code translated into a similar color.
gui_pixmap cameraPixmap = NULL;
gui_pixmap filterPixmap = NULL;
gui_rectangle behaviorViews[NUMBEHAVIORS];				//colored bars that show information about the present states of each behavior.
gui_checkbox cameraCheckBox;							//is the camera image constantly updated?
gui_checkbox filterCheckBox;							//is the filter image constantly updated?
int16 IRsensors[8];										//IR input values for each step.
uint8 *rawImage;										//that camera's raw bitmap.
uint8 filteredImageCodes[K6300_WIDTH][K6300_HEIGHT];	//map of camera pixels translated into object-type codes.

int16 cpuCost = 0;										//cpu cost returned by the most recent timeStep.
bool turnOnScanSearchLeft, turnOnScanSearchRight;		//trigger a ScanSearch behavior in one direction or the other.
int16 postScanTurnAmount = 0;							//what direction and how far should the robot turn after doing a scan?
int16 leftAvoidVisualHeight = 16;						//how high is the left half of the avoidVisual trapezoid.
int16 rightAvoidVisualHeight = 16;						//how high is the right half of the avoidVisual trapezoid.
int16 desiredFeederHeight = 0;							//what is the approximate height of the feeder that the robot is turning towards after a scan?
bool groundVisible;										//is there any ground visible at all?  Even one pixel will do.
int16 leftGroundRepulsion, rightGroundRepulsion;		//orients the robot toward open floor space.  Has a higher value is ground is less present on its side of the view.
int16 difGroundRepulsion;								//leftGroundRepulsion - rightGroundRepulsion.  Based on the sign of this value, the balance of ground can be determined.
int16 leftWallRepulsion, rightWallRepulsion;			//orients the robot away from the arena's side walls (and thus toward the middle of the arena.
int16 difWallRepulsion;									//leftWallRepulsion - rightWallRepulsion, like difGroundRepulsion.
int32 groundValue;										//a measure of the available ground visible.
int32 skyBalance;										//measures whether more sky is visible to the left or the right.
Box feederOnBound;										//bounding box around the closest green feeder.
Box feederOffBound;										//bounding box around the closest red feeder.
Box feederBoxBound;										//bounding box around the closest feeder box.
int16 oldLandmarkGoalAge;								//at what age is a landmark assumed to be really old (since it was last closely approached by the robot)
int8 feederOnDistance;									//distance to the nearest green feeder.
int8 feederOffDistance;									//distance to the nearest red feeder.
Box whiteBoxBound;										//bounding box around the white box.
Box magentaBoxBound;									//bounding box around the magenta box.
Box yellowBoxBound;										//bounding box around the yellow box.
Box cyanBoxBound;										//bounding box around the cyan box.
Box diskBound;											//bounding box around the magenta disk.
Box redCornerBound;										//bounding box around the cyan corners.
Box cyanCornerBound;									//bounding box around the red corners.
Box zeroDimBox;											//all values = 0
Box insideOutBox;										//left = max, right = 0, top = max, bottom = 0
int16 groundTop;										//highest ground presently visible
int16 skyBottom;										//lowest sky presently visible
bool initializeAllBehaviors = false;					//tells the behaviors to erase all their static variables.
bool initGlance = false;								//tells the glance behaviors to erase their static variables.
bool initFeederOrient = false;							//tells the feederOrient behavior to erase its static variables.
bool tryRightGlanceAgain = false;						//glances again after the first glance looked straight into a wall.
bool tryLeftGlanceAgain = false;						//glances again after the first glance looked straight into a wall.
bool feederOnDetected = false;							//has a green feeder been detected?
bool feederOffDetected = false;							//has a red feeder been detected?
bool feederBoxDetected = false;							//has a feeder box been detected?
bool scanning = false;									//is the robot performing an association scan?
bool glanceCounting = false;							//is a glance behavior in the middle of pacing past the end of an object?
int16 recentlyFeederOrienting = 0;						//has it been a while since the robot oriented towards a feeder?
int16 recentlyFed = 0;									//has it been a while since the robot fed?
bool nearFeederOff;										//is a red feeder very nearby?
bool seekingFeeder = false;								//is the robot scanning for a feeder?
bool seekingCloseFeeder = false;						//is the robot exclusively scanning for a nearby feeder?
bool killOtherGlance = false;							//should one glance behavior be initialized?  Set by the other glance behavior.
int8 latentTrapAvoidance = LEFT_AND_RIGHT;				//after avoiding a trap this will prevent the robot from turning back towards the trap.
bool triggerEscapeStuck = false;						//trigger the escapeStuck behavior regardless of its own sensors and trigger.
int8 boxPeriferal[26];									//lookup table of distances for boxes based on box attributes.
int8 boxCentered[26];									//lookup table of distances for boxes based on box attributes.
int8 diskPeriferal[18];									//lookup table of distances for disk based on disk attributes.
int8 diskCentered[18];									//lookup table of distances for disk based on disk attributes.
int8 cornerPeriferal[13];								//lookup table of distances for corners based on corner attributes.
int8 cornerCentered[13];								//lookup table of distances for corners based on corner attributes.
int8 feederPeriferal[20];								//lookup table of distances for feeder based on feeder attributes.
int8 feederCentered[20];								//lookup table of distances for feeder based on feeder attributes.

uint8 sky[3]			= {98, 180, 255};				//color values of sky
uint8 ground[3]			= {255, 213, 123};				//color values of ground
uint8 feeder_on[3]		= {74, 255, 74};				//color values of a green feeder
uint8 feeder_off[3]		= {255, 74, 74};				//color values of a red feeder
uint8 feederBox[3]		= {172, 172, 238};				//color values of feeder box
uint8 sideWall[3]		= {50, 50, 190};				//color values of side walls

uint8 whiteBox[3]		= {255, 255, 255};				//color values of white box
uint8 magentaBox[3]		= {255, 123, 255};				//color values of magenta box
uint8 yellowBox[3]		= {255, 255, 32};				//color values of yellow box
uint8 cyanBox[3]		= {106, 222, 180};				//color values of cyan box
uint8 disk[3]			= {222, 131, 222};				//color values of magenta disk
uint8 red_corner[3]		= {255, 70, 107};				//color values of red corners
uint8 cyan_corner[3]	= {0, 172, 172};				//color values of cyan corners	

WorldKnowledge theWorldKnowledge;						//gathered knowledge about the world
ImmediateViewKnowledge theImmediateViewKnowledge;		//knowledge of the present camera view
PresentTimeStepData thePresentTimeStepData;				//settings for the present timestep

//  LANDMARK MEMORY  *******************************************************
//This class creates a data structure about one landmark.  The information stored mainly pertains to the landmark's
//relationship to other landmarks in terms of visibility and distance, as well as feeders.
#define MAXNUMASSOCIATIONS 7

class LandmarkMemory
{
	private:
		int16 numAssociations;
		int16 associatedLandmarkIds[MAXNUMASSOCIATIONS];
	
	public:
		int16 id;
		bool feederAssociation;
		
		LandmarkMemory();
		~LandmarkMemory();
		void Copy(const LandmarkMemory &lm);
		const LandmarkMemory &operator=(const LandmarkMemory &right);
		void AddAssociation(int16 id);
		bool IsLandmarkAssociated(int16 id);
};

LandmarkMemory::LandmarkMemory()
{
	numAssociations = 0;
	
	for (int8 i = 0; i < MAXNUMASSOCIATIONS; i++)
		associatedLandmarkIds[i] = BLANK;
	
	id = 0;
	feederAssociation = false;
}

LandmarkMemory::~LandmarkMemory()
{
}

void LandmarkMemory::Copy(const LandmarkMemory &lm)
{
	numAssociations = lm.numAssociations;
	
	for (int8 i = 0; i < MAXNUMASSOCIATIONS; i++)
		associatedLandmarkIds[i] = lm.associatedLandmarkIds[i];
		
	id = lm.id;
	feederAssociation = lm.feederAssociation;
}

const LandmarkMemory &LandmarkMemory::operator=(const LandmarkMemory &right)
{
	if (&right != this)
		Copy(right);
	
	return *this;
}

//adds an association to a landmark, meaning that another landmark can be viewed from this landmark.
void LandmarkMemory::AddAssociation(int16 id)
{
	if (numAssociations < MAXNUMASSOCIATIONS)
	{
		associatedLandmarkIds[numAssociations] = id;
		
		numAssociations++;
	}
}

//checks to see if a landmark has been associated yet.  This consists of going up to the
//landmark and doing a full circle scan to see what is visible from that landmark's location.
bool LandmarkMemory::IsLandmarkAssociated(int16 id)
{
	for (int8 i = 0; i < numAssociations; i++)
		if (associatedLandmarkIds[i] == id)
			return true;
	
	return false;
}
//  LANDMARK MEMORY  *******************************************************
//  BEHAVIOR  *******************************************************
//each kind of behavior is stored in a behavior class instance.  The RunBehavior
//function calls the correct specific behavior function for that behavior type.
#define NOBEH					20

#define AVOID_IR				0
#define AVOIDVISUAL				1
#define AVOIDTRAP				2
#define AVOIDDEADEND			3
#define FORWARD					4
#define GLANCERIGHT				5
#define GLANCELEFT				6
#define SCANSEARCH				7
#define LANDMARKORIENT			8
#define FEEDERORIENT			9
#define SCANASSOCIATE			10
#define POSTFEEDSCANASSOCIATE	11
#define ESCAPECORNER			12
#define ESCAPESTUCK				13

class Behavior
{
	private:
		int16 trigger;								//threshhold to trigger behavior as activation rises
		int16 cutoff;								//threshhold to cutoff behavior as activation drops
		int16 decayRate;							//rate of activation decay once triggered
		int16 activation;							//present activation for this particular behavior
		bool inhibitList[NUMBEHAVIORS];				//which behaviors does this behavior inhibit
		Behavior *otherBehaviorPtrs[NUMBEHAVIORS];	//pointers to all the other behaviors
		
		bool RunAvoid_IR(int16 &left, int16 &right);
		bool RunAvoidVisual(int16 &left, int16 &right);
		bool RunAvoidTrap(int16 &left, int16 &right);
		bool RunAvoidDeadend(int16 &left, int16 &right);
		bool RunForward(int16 &left, int16 &right);
		bool RunGlanceRight(int16 &left, int16 &right);
		bool RunGlanceLeft(int16 &left, int16 &right);
		bool RunScanSearch(int16 &left, int16 &right);
		bool RunLandmarkOrient(int16 &left, int16 &right);
		bool RunFeederOrient(int16 &left, int16 &right);
		bool RunScanAssociate(int16 &left, int16 &right);
		bool RunPostFeedScanAssociate(int16 &left, int16 &right);
		bool RunEscapeCorner(int16 &left, int16 &right);
		bool RunEscapeStuck(int16 &left, int16 &right);
		
	public:
		int16 id;									//numerical code for the kind of behavior this is (see list above)
		bool active;								//is this behavior presently active
		bool inhibited;								//has this behavior been inhibited by a higher level behavior
		
		Behavior();
		~Behavior();
		void Copy(Behavior &b);
		void Initialize(int16 theId, int16 theTrigger, int16 theCutoff, int16 theDecayRate, bool theInhibits[7], Behavior *theBehaviors[NUMBEHAVIORS]);
		
		bool RunBehavior(int16 &left, int16 &right);
};

Behavior::Behavior()
{
}

Behavior::~Behavior()
{
}

void Behavior::Copy(Behavior &b)
{
	trigger = b.trigger;
	cutoff = b.cutoff;
	decayRate = b.decayRate;
	activation = b.activation;
	
	for (int8 i = 0; i < NUMBEHAVIORS; i++)
	{
		inhibitList[i] = b.inhibitList[NUMBEHAVIORS];
		otherBehaviorPtrs[i] = b.otherBehaviorPtrs[i];
	}
	
	id = b.id;
	inhibited = b.inhibited;
	active = b.active;
}

void Behavior::Initialize(int16 theId, int16 theTrigger, int16 theCutoff, int16 theDecayRate,
				bool theInhibits[NUMBEHAVIORS], Behavior *theBehaviors[NUMBEHAVIORS])
{
	id = theId;
	
	trigger = theTrigger;
	cutoff = theCutoff;
	decayRate = theDecayRate;
	activation = 0;
	
	for (int8 i = 0; i < NUMBEHAVIORS; i++)
	{
		inhibitList[i] = theInhibits[i];
		otherBehaviorPtrs[i] = theBehaviors[i];
	}
	otherBehaviorPtrs[id] = NULL;
	
	inhibited = false;
	active = false;
}

//Runs the appropriate behavior function for this particular behavior
bool Behavior::RunBehavior(int16 &left, int16 &right)
{
	if (id != NOBEH)
	{
		gui_rectangle_change_size(behaviorViews[id], 100, 5);
		gui_rectangle_change_color(behaviorViews[id], GUI_BLACK);
	}
	
	//if the behavior is being inhibited, simply exit out without running the behavior's routines.
	if (id == NOBEH || inhibited)
	{
		activation = 0;
		active = false;
		return false;
	}
	
	bool returnValue = false;
	
	switch (id)
	{
		case AVOID_IR:
			if (RunAvoid_IR(left, right))
				for (int8 i = 0; i < NUMBEHAVIORS; i++)
					if (inhibitList[i])	//It can't just set 'inhibited' to the list
										//value because higher level behaviors may
										//have inhibited a lower behavior and it
										//doesn't want to undo that effect.
						otherBehaviorPtrs[i]->inhibited = true;
			break;
		case AVOIDVISUAL:
			if (RunAvoidVisual(left, right))
				for (int8 i = 0; i < NUMBEHAVIORS; i++)
					if (inhibitList[i])
						otherBehaviorPtrs[i]->inhibited = true;
			break;
		case AVOIDTRAP:
			if (RunAvoidTrap(left, right))
				for (int8 i = 0; i < NUMBEHAVIORS; i++)
					if (inhibitList[i])
						otherBehaviorPtrs[i]->inhibited = true;
			break;
		case AVOIDDEADEND:
			if (RunAvoidDeadend(left, right))
				for (int8 i = 0; i < NUMBEHAVIORS; i++)
					if (inhibitList[i])
						otherBehaviorPtrs[i]->inhibited = true;
			break;
		case FORWARD:
			if (RunForward(left, right))
				for (int8 i = 0; i < NUMBEHAVIORS; i++)
					if (inhibitList[i])
						otherBehaviorPtrs[i]->inhibited = true;
			break;
		case GLANCERIGHT:
			if (RunGlanceRight(left, right))
				for (int8 i = 0; i < NUMBEHAVIORS; i++)
					if (inhibitList[i])
						otherBehaviorPtrs[i]->inhibited = true;
			break;
		case GLANCELEFT:
			if (RunGlanceLeft(left, right))
				for (int8 i = 0; i < NUMBEHAVIORS; i++)
					if (inhibitList[i])
						otherBehaviorPtrs[i]->inhibited = true;
			break;
		case SCANSEARCH:
			if (RunScanSearch(left, right))
				for (int8 i = 0; i < NUMBEHAVIORS; i++)
					if (inhibitList[i])
						otherBehaviorPtrs[i]->inhibited = true;
			break;
		case LANDMARKORIENT:
			if (RunLandmarkOrient(left, right))
			{
				for (int8 i = 0; i < NUMBEHAVIORS; i++)
					if (inhibitList[i])
						otherBehaviorPtrs[i]->inhibited = true;
				
				returnValue = true;
			}
			break;
		case FEEDERORIENT:
			if (RunFeederOrient(left, right))
			{
				for (int8 i = 0; i < NUMBEHAVIORS; i++)
					if (inhibitList[i])
						otherBehaviorPtrs[i]->inhibited = true;
				
				returnValue = true;
			}
			break;
		case SCANASSOCIATE:
			if (RunScanAssociate(left, right))
			{
				for (int8 i = 0; i < NUMBEHAVIORS; i++)
					if (inhibitList[i])
						otherBehaviorPtrs[i]->inhibited = true;
				
				returnValue = true;
			}
			break;
		case POSTFEEDSCANASSOCIATE:
			if (RunPostFeedScanAssociate(left, right))
			{
				for (int8 i = 0; i < NUMBEHAVIORS; i++)
					if (inhibitList[i])
						otherBehaviorPtrs[i]->inhibited = true;
				
				returnValue = true;
			}
			break;
		case ESCAPECORNER:
			if (RunEscapeCorner(left, right))
				for (int8 i = 0; i < NUMBEHAVIORS; i++)
					if (inhibitList[i])
						otherBehaviorPtrs[i]->inhibited = true;
			break;
		case ESCAPESTUCK:
			if (RunEscapeStuck(left, right))
				for (int8 i = 0; i < NUMBEHAVIORS; i++)
					if (inhibitList[i])
						otherBehaviorPtrs[i]->inhibited = true;
			break;
	}
	
	return returnValue;
}

//Braitenberg reflexes, modified slightly when orienting towards a feeder
bool Behavior::RunAvoid_IR(int16 &left, int16 &right)
{	
	int16 alphaLeft = left;
	int16 alphaRight = right;
	bool returnValue = false;
	
	//interconnection matrix between IR proximity sensors and motors
	static int32 InterconnLeft[8] = {4, 4, 6, -18, -15, -5, 5, 3};
	static int32 InterconnRight[8] = {-5, -15, -18, 6, 4, 4, 3, 5};
	
	if (initializeAllBehaviors)
	{
		activation = 0;
		active = false;
		
		return false;
	}
	
	if (!otherBehaviorPtrs[FEEDERORIENT]->active)
		for (int8 i = 0; i <= 7; i++)
		{
			left += (IRsensors[i] * InterconnLeft[i]) / 400;
			right += (IRsensors[i] * InterconnRight[i]) / 400; 
		}
	else	//the robot shouldn't be veering away from side objects as much as usual because it has
			//a specific objective and should only be concerned with obstructions in its desired path.
	{
		if (IRsensors[0] > 500)
		{
			left += (IRsensors[0] * InterconnLeft[0]) / 600;
			right += (IRsensors[0] * InterconnRight[0]) / 600;
		}
		if (IRsensors[5] > 500)
		{
			left += (IRsensors[5] * InterconnLeft[5]) / 600;
			right += (IRsensors[5] * InterconnRight[5]) / 600;
		}
		
		if (IRsensors[1] > 500)
		{
			left += (IRsensors[1] * InterconnLeft[1]) / 600;
			right += (IRsensors[1] * InterconnRight[1]) / 600;
		}
		if (IRsensors[4] > 500)
		{
			left += (IRsensors[4] * InterconnLeft[4]) / 600;
			right += (IRsensors[4] * InterconnRight[4]) / 600;
		}
		
		left += (IRsensors[2] * InterconnLeft[2]) / 600;
		right += (IRsensors[2] * InterconnRight[2]) / 600;
		left += (IRsensors[3] * InterconnLeft[3]) / 600;
		right += (IRsensors[3] * InterconnRight[3]) / 600;
		
		if (left < 0)
			left = 0;
		if (right < 0)
			right = 0;
	}
	
	if (alphaLeft != left || alphaRight != right)
		returnValue = true;
	
	if (returnValue)
	{
		gui_rectangle_change_size(behaviorViews[AVOID_IR], 100, 5);
		gui_rectangle_change_color(behaviorViews[AVOID_IR], GUI_BLUE);
	}
	
	return returnValue;
}

//In the filter view you will see a large blue trapezoidal area with two smaller blue side areas.
//Pixels that are not ground colored in the central area will push the robot the opposite direction
//Pixels in the side areas will prevent the robot from turning in toward that side
//There is also some attraction to the center of mass of the entire ground area visible
bool Behavior::RunAvoidVisual(int16 &left, int16 &right)
{
	bool runIt = false;										//should the avoidVisual areas be checked for ground pixels?
	int16 numTotalTrapezoidPixels = 0;						//total number of ground pixels in the central trapezoid.
	int16 numGroundTrapezoidPixels = 0;						//number of ground pixels in the central trapezoid.
	int32 veerLeft = 0;										//repulsion from the right.
	int32 veerRight = 0;									//repulsion from the left.
	int32 avoidLeft = 0;									//do not turn left.
	int32 avoidRight = 0;									//do not turn right.
	int16 leftHalf = 22 - (leftAvoidVisualHeight / 2);		//used to push the area being checked out as the it descends down the trapezoid.
	int16 rightHalf = 22 - (rightAvoidVisualHeight / 2);	//used to push the area being checked out as the it descends down the trapezoid.
	bool returnValue = false;
	bool normalizeSpeed = true;								//this will bring the robot's speed back up to maximum at the end of the function
	bool trapezoidSaturated = false;						//is the central trapezoid almost entirely devoid of ground pixels?
	static int16 latentAvoidLeft = 0;						//after passing an object on the left, this keeps the robot from turning back into it.
	static int16 latentAvoidRight = 0;						//after passing an object on the right, this keeps the robot from turning back into it.
	static int8 priorValues[2][4];							//a recent memory of past avoidVisual results will help determine if the robot is oscillating.
	int8 directionDifference[4];							//uses priorValues[][] to check for an oscillating pattern.
	static int16 cycling = 0;								//accumulates cycling data.
	
	if (initializeAllBehaviors)
	{
		activation = 0;
		active = false;
		
		cycling = 0;
		latentAvoidLeft = 0;
		latentAvoidRight = 0;
		
		for (int8 i = 0; i < 4; i++)
			priorValues[LEFT][i] = priorValues[RIGHT][i] = 0;
		
		return false;
	}
	
	//decrease AV height as the robot approaches a feeder
	if (otherBehaviorPtrs[FEEDERORIENT]->active)
	{
		if (feederOnDetected)
		{
			if (feederOnDistance < 6)
			{
				if (feederOnBound.height / feederOnBound.width < 5)
				{
					if (feederOnDistance <= 3)
					{
						if (leftAvoidVisualHeight > 6)
							leftAvoidVisualHeight = 6;
						if (rightAvoidVisualHeight > 6)
							rightAvoidVisualHeight = 6;
					}
					else if (feederOnDistance == 4)
					{
						if (leftAvoidVisualHeight > 10)
							leftAvoidVisualHeight = 10;
						if (rightAvoidVisualHeight > 10)
							rightAvoidVisualHeight = 10;
					}
					else if (feederOnDistance == 5)
					{
						if (leftAvoidVisualHeight > 14)
							leftAvoidVisualHeight = 14;
						if (rightAvoidVisualHeight > 14)
							rightAvoidVisualHeight = 14;
					}
				}
			}
		}
	}
	
	//check to see if the trapezoid and side areas should be checked for ground pixels
	if (!otherBehaviorPtrs[GLANCELEFT]->active && !otherBehaviorPtrs[GLANCERIGHT]->active)
	{
		if (otherBehaviorPtrs[FEEDERORIENT]->active)
		{
			if (feederOnDetected && feederOnBound.height < 20)	//is feeder sufficiently far away?  Must check for feederOnDetected because the robot may
				runIt = true;									//just be orienting towards a feederBox, in which avoidVisual should run as usual.
			else if (!feederOnDetected)
				runIt = true;
		}
		else if (otherBehaviorPtrs[LANDMARKORIENT]->active)
		{
			if (!theWorldKnowledge.needsAssociating[thePresentTimeStepData.presentLandmarkGoal - 8])	//don't even consider turning off avoidVisual if the landmark has already been associated.
				runIt = true;
			else if (thePresentTimeStepData.presentLandmarkGoal < RED_CORNER)
			{
				if (thePresentTimeStepData.presentLandmarkGoalBox.height < 19)	//is the landmark (box or disk) sufficiently far away?
					runIt = true;
			}
			else if (thePresentTimeStepData.presentLandmarkGoalBox.width < 6)	//is the landmark (corner) sufficiently far away?
					runIt = true;
		}
		else runIt = true;	//not feeder orienting and not landmark orienting
	}
	
	if (runIt)
	{
		for (int8 i = 0; i < 3; i++)
		{
			priorValues[LEFT][i] = priorValues[LEFT][i + 1];
			priorValues[RIGHT][i] = priorValues[RIGHT][i + 1];
		}
		
		for (int8 y = K6300_HEIGHT - leftAvoidVisualHeight; y < K6300_HEIGHT; y++)
		{
			//check the left half of the central area for repulsion against the left side
			for (int8 x = k6300_halfWidth - leftHalf; x < k6300_halfWidth; x++)
			{
				numTotalTrapezoidPixels++;
				
				if (filteredImageCodes[x][y] == GROUND)
					numGroundTrapezoidPixels++;
				else if (!otherBehaviorPtrs[LANDMARKORIENT]->active ||
					(!theWorldKnowledge.needsAssociating[thePresentTimeStepData.presentLandmarkGoal - 8] ||
					filteredImageCodes[x][y] != thePresentTimeStepData.presentLandmarkGoal))
					veerRight += ((leftHalf + 2) - (k6300_halfWidth - x)) * (y - 43);
			}
			
			//check the left small area for prevention of turning left
			if (y >= 53)
			{
				for (int8 x = k6300_halfWidth - leftHalf - 5; x <= k6300_halfWidth - leftHalf - 1; x++)
					if ((!otherBehaviorPtrs[LANDMARKORIENT]->active && filteredImageCodes[x][y] != GROUND) ||
						(otherBehaviorPtrs[LANDMARKORIENT]->active &&
						((!theWorldKnowledge.needsAssociating[thePresentTimeStepData.presentLandmarkGoal - 8] && filteredImageCodes[x][y] != GROUND) ||
						(theWorldKnowledge.needsAssociating[thePresentTimeStepData.presentLandmarkGoal - 8] &&
						filteredImageCodes[x][y] != thePresentTimeStepData.presentLandmarkGoal && filteredImageCodes[x][y] != GROUND))))
						avoidLeft++;
			}
			
			if (y % 2 == 0)
				leftHalf++;
		}
		
		for (int8 y = K6300_HEIGHT - rightAvoidVisualHeight; y < K6300_HEIGHT; y++)
		{
			//check the right half of the central area for repulsion against the right side
			for (int8 x = k6300_halfWidth; x < k6300_halfWidth + rightHalf; x++)
			{
				numTotalTrapezoidPixels++;
				
				if (filteredImageCodes[x][y] == GROUND)
					numGroundTrapezoidPixels++;
				else if (!otherBehaviorPtrs[LANDMARKORIENT]->active ||
					(!theWorldKnowledge.needsAssociating[thePresentTimeStepData.presentLandmarkGoal - 8] ||
					filteredImageCodes[x][y] != thePresentTimeStepData.presentLandmarkGoal))
					veerLeft += ((rightHalf + 2) - (x - k6300_halfWidth)) * (y - 43);
			}
			
			//check the right small area for prevention of turning right
			if (y >= 53)
			{
				for (int8 x = k6300_halfWidth + rightHalf; x < k6300_halfWidth + rightHalf + 5; x++)
					if ((!otherBehaviorPtrs[LANDMARKORIENT]->active && filteredImageCodes[x][y] != GROUND) ||
						(otherBehaviorPtrs[LANDMARKORIENT]->active &&
						((!theWorldKnowledge.needsAssociating[thePresentTimeStepData.presentLandmarkGoal - 8] && filteredImageCodes[x][y] != GROUND) ||
						(theWorldKnowledge.needsAssociating[thePresentTimeStepData.presentLandmarkGoal - 8] &&
						filteredImageCodes[x][y] != thePresentTimeStepData.presentLandmarkGoal && filteredImageCodes[x][y] != GROUND))))
						avoidRight++;
			}
			
			if (y % 2 == 0)
				rightHalf++;
		}
		
		//add some attraction to the center of mass of the entire visible ground area and the visible sky.  Work wall repulsion into it also.
		if (fabs(difGroundRepulsion) > 100 && !otherBehaviorPtrs[FEEDERORIENT]->active && !otherBehaviorPtrs[LANDMARKORIENT]->active)
		{
			veerLeft += rightGroundRepulsion;
			veerRight += leftGroundRepulsion;
			
			veerLeft += rightWallRepulsion;
			veerRight += leftWallRepulsion;
			
			veerLeft -= skyBalance;
			veerRight += skyBalance;
		}
		
		//if both side areas have avoidance pixels but the center doesn't, the robot might be trying to squeeze through a small crevice.
		//in that case, don't let the side areas prevent turning because the robot needs to be able to manuvere in the available space.
		if ((float)numGroundTrapezoidPixels / (float)numTotalTrapezoidPixels < .2)
			trapezoidSaturated = true;
		if (avoidLeft && avoidRight && !trapezoidSaturated)
		{
			avoidLeft = avoidRight = 0;
			latentAvoidLeft = latentAvoidRight = 0;
		}
		
		//bring the motor values up to a baseline before altering them with the AV changes.
		if (veerLeft > 500 && veerLeft > veerRight && left > right)
			right = left;
		if (veerRight > 500 && veerRight > veerLeft && right > left)
			left = right;
		
		//alter the left and right values according to the results of the previous calculations
		if (veerLeft > veerRight)
		{
			left -= veerLeft / 100;
			if (left < 6)
				left = 6;
			
			cycling = 0;
			returnValue = true;
		}
		else if (veerRight > veerLeft)
		{
			right -= veerRight / 100;
			if (right < 6)
				right = 6;
			
			cycling = 0;
			returnValue = true;
		}
	}
	
	//if a trap was recently avoided, don't turn back into it, even if the side areas are visibly clear of obstructions.
	if (latentTrapAvoidance == LEFT)
		avoidLeft = 1;
	else if (latentTrapAvoidance == RIGHT)
		avoidRight = 1;
	
	//this makes sure the robot doesn't turn in towards a direction that had nonground in the small side areas
	if (left < right)
	{
		if (avoidLeft > 0)
		{
			left = right;
			
			if (cpuCost < 15)
				latentAvoidLeft = 28;
			else if (cpuCost < 25)
				latentAvoidLeft = 23;
			else if (cpuCost < 35)
				latentAvoidLeft = 20;
			else if (cpuCost < 50)
				latentAvoidLeft = 18;
			else if (cpuCost < 65)
				latentAvoidLeft = 16;
			else latentAvoidLeft = 14;
		}
		else if (latentAvoidLeft > 5)
			left = right;
		else if (latentAvoidLeft > 0 && (right - left) > (8 - latentAvoidLeft))
			left = right - (8 - latentAvoidLeft);
	}
	latentAvoidLeft--;
	
	if (right < left)
	{
		if (avoidRight > 0)
		{
			right = left;
			
			if (cpuCost < 15)
				latentAvoidRight = 28;
			else if (cpuCost < 25)
				latentAvoidRight = 23;
			else if (cpuCost < 35)
				latentAvoidRight = 20;
			else if (cpuCost < 50)
				latentAvoidRight = 18;
			else if (cpuCost < 65)
				latentAvoidRight = 16;
			else latentAvoidRight = 14;
		}
		else if (latentAvoidRight > 5)
			right = left;
		else if (latentAvoidRight > 0 && (left - right) > (8 - latentAvoidRight))
			right = left - (8 - latentAvoidRight);
	}
	latentAvoidRight--;
	
	//normalizing the speed keeps the robot at full throttle
	if (normalizeSpeed && left != 20 && right != 20)
	{
		if (left > right)
		{
			right += 20 - left;
			left += 20 - left;
		}
		else
		{
			left += 20 - right;
			right += 20 - right;
		}
	}
	
	//record the present motor speeds to check for oscillations in the future.
	priorValues[LEFT][3] = left;
	priorValues[RIGHT][3] = right;
	
	for (int8 i = 0; i < 4; i++)
		directionDifference[i] = priorValues[LEFT][i] - priorValues[RIGHT][i];
	
	//check for oscillations, but only if facing a wall (which seems to be a common cause of osciallations).  If so, run escapeStuck.
	if (trapezoidSaturated &&
				(directionDifference[0] < 0 && directionDifference[1] > 0 && directionDifference[2] < 0 && directionDifference[3] > 0) ||
				(directionDifference[0] > 0 && directionDifference[1] < 0 && directionDifference[2] > 0 && directionDifference[3] < 0))
	{
		cycling = 3;
		
		triggerEscapeStuck = true;
	}
	
	if (returnValue)
	{
		gui_rectangle_change_size(behaviorViews[AVOIDVISUAL], 100, 5);
		gui_rectangle_change_color(behaviorViews[AVOIDVISUAL], GUI_BLUE);
	}
	
	return returnValue;
}

//trys to detect when the robot is aiming for an opening that is too small to squeeze through and aims around the obstruction.
bool Behavior::RunAvoidTrap(int16 &left, int16 &right)
{
	int8 x;									//used in for loops.
	int8 xAnnex;							//used to search for a central piece of ground.
	int8 centralPointX;						//horizontal center of a central piece of ground.
	int8 startY;							//what vertical level to start searching at.
	int8 height;							//how much vertical space to search.
	int8 groundSaturationPoint;				//amount of ground necessary to be considered open ground.
	int8 neededHorizontalSpace;				//minimum amount of horizontal space need for the robot to squeeze through.
	int8 directionChange;					//how much to turn to avoid the trap.
	int8 ySliceGroundAmount;				//amount of ground in a slice of the region being checked.
	int8 yChunkStart = 0;					//which vertical area is being checked for open ground.
	bool keepChecking;						//should avoidTrap keep checking for a trap?
	bool noCentralGround;					//is there no central ground at all?
	int8 groundLeftEdge, groundRightEdge;	//edges of the open ground area.
	int16 wallToLeft = 0;					//is there wall to the left?  If so and a trap is detected, steer away from the wall.
	int16 wallToRight = 0;					//is there wall to the right?  If so and a trap is detected, steer away from the wall.
											//if there aren't any walls, then AT will steer towards open ground.
	static int8 direction;					//what direction to turn?
	static int8 stopTurning;				//when does AT stop turning and start holding the robot from turning back into the trap?
	
	if (initializeAllBehaviors)
	{
		activation = 0;
		active = false;
		
		direction = 0;
		stopTurning = 0;
		
		return false;
	}
	
	if (!active)
	{
		if (otherBehaviorPtrs[FEEDERORIENT]->active)
		{
			if (feederOnDistance <= 3)
				yChunkStart = 4;
			else if (feederOnDistance == 4)
				yChunkStart = 3;
			else if (feederOnDistance == 5)
				yChunkStart = 2;
			else if (feederOnDistance == 6)
				yChunkStart = 1;
		}
		else if (otherBehaviorPtrs[LANDMARKORIENT]->active)
		{
			if (theImmediateViewKnowledge.landmarkDistances[thePresentTimeStepData.presentLandmarkMemoryIndex] <= 3)
				yChunkStart = 4;
			else if (theImmediateViewKnowledge.landmarkDistances[thePresentTimeStepData.presentLandmarkMemoryIndex] == 4)
				yChunkStart = 3;
			else if (theImmediateViewKnowledge.landmarkDistances[thePresentTimeStepData.presentLandmarkMemoryIndex] == 5)
				yChunkStart = 2;
			else if (theImmediateViewKnowledge.landmarkDistances[thePresentTimeStepData.presentLandmarkMemoryIndex] == 6)
				yChunkStart = 1;
		}
			
		for (int8 yChunk = yChunkStart; yChunk < 4; yChunk++)
		{
			noCentralGround = false;
			
			switch (yChunk)
			{
				case 0:
					startY = 40;
					height = 3;
					groundSaturationPoint = 2;
					neededHorizontalSpace = 14;
					directionChange = 10;
					break;
				case 1:
					startY = 43;
					height = 4;
					groundSaturationPoint = 3;
					neededHorizontalSpace = 16;
					directionChange = 8;
					break;
				case 2:
					startY = 47;
					height = 4;
					groundSaturationPoint = 3;
					neededHorizontalSpace = 18;
					directionChange = 6;
					break;
				case 3:
					startY = 51;
					height = 5;
					groundSaturationPoint = 3;
					neededHorizontalSpace = 20;
					directionChange = 4;
					break;
			}
			
			//try to find a central piece of ground from which to search outwards for its boundries
			x = k6300_halfWidth - 1;
			xAnnex = 1;
			keepChecking = true;
			do
			{
				ySliceGroundAmount = 0;
				for (int8 y = startY; y < startY + height; y++)
					if (filteredImageCodes[x][y] == GROUND)
						ySliceGroundAmount++;
				
				if (ySliceGroundAmount < groundSaturationPoint)
				{
					x += xAnnex;
					if (xAnnex > 0)
						xAnnex -= (xAnnex * 2 + 1);
					else xAnnex -= (xAnnex * 2 - 1);
					
					if (x < k6300_halfWidth - 10 || x > k6300_halfWidth + 9)
					{
						noCentralGround = true;
						keepChecking = false;
					}
				}
				else
				{
					centralPointX = x;
					keepChecking = false;
				}
			}while (keepChecking);
			
			//if a central piece of ground is found, proceed to measure its horizontal width
			if (!noCentralGround)
			{
				//search for the left boundry
				x = centralPointX;
				keepChecking = true;
				do
				{
					ySliceGroundAmount = 0;
					for (int8 y = startY; y < startY + height; y++)
						if (filteredImageCodes[x][y] == GROUND)
							ySliceGroundAmount++;
					
					if (ySliceGroundAmount >= groundSaturationPoint)
						x--;
					else keepChecking = false;
				}while (keepChecking);
				
				groundLeftEdge = x;
				
				//search for the right boundry
				x = centralPointX + 1;
				keepChecking = true;
				do
				{
					ySliceGroundAmount = 0;
					for (int8 y = startY; y < startY + height; y++)
						if (filteredImageCodes[x][y] == GROUND)
							ySliceGroundAmount++;
					
					if (ySliceGroundAmount >= groundSaturationPoint)
						x++;
					else keepChecking = false;
				}while (keepChecking);
				
				groundRightEdge = x;
				
				if (groundRightEdge - groundLeftEdge + 1 < neededHorizontalSpace)	//is the piece of ground too thin to get through?
				{
					activation++;
					
					if (activation >= trigger)
					{
						for (int8 y = 0; y < K6300_HEIGHT; y++)
							for (int8 x = k6300_halfWidth - 15; x < k6300_halfWidth; x++)
								if (x >= 0 && filteredImageCodes[x][y] == BLANK)
									wallToLeft++;
									
						for (int8 y = 0; y < K6300_HEIGHT; y++)
							for (int8 x = k6300_halfWidth; x < k6300_halfWidth + 15; x++)
								if (x >= 0 && filteredImageCodes[x][y] == BLANK)
									wallToRight++;
						
						if (wallToLeft > wallToRight && wallToLeft > 25)			//try to steer away from a wall to the left
						{
								direction = directionChange;		//turn right
								latentTrapAvoidance = LEFT;
						}
						else if (wallToRight > wallToLeft && wallToRight > 25)		//try to steer away from a wall to the right
						{
								direction = -directionChange;	//turn left
								latentTrapAvoidance = RIGHT;
						}
						else
						{
							if (difGroundRepulsion > 0)			//leftGroundRepulsion is greater than rightGroundRepulsion, so go right
							{
								direction = directionChange;		//turn right
								latentTrapAvoidance = LEFT;
							}
							else
							{
								direction = -directionChange;	//turn left
								latentTrapAvoidance = RIGHT;
							}
						}
																
						if (cpuCost < 15)
						{
							activation = 50;
							stopTurning = 25;
						}
						else if (cpuCost < 25)
						{
							activation = 40;
							stopTurning = 18;
						}
						else if (cpuCost < 35)
						{
							activation = 35;
							stopTurning = 12;
						}
						else if (cpuCost < 65)
						{
							activation = 25;
							stopTurning = 8;
						}
						else if (cpuCost < 80)
						{
							activation = 20;
							stopTurning = 6;
						}
						else
						{
							activation = 15;
							stopTurning = 4;
						}
						
						active = true;
					}
					
					if (!active)
					{
						gui_rectangle_change_size(behaviorViews[AVOIDTRAP], activation, 5);
						gui_rectangle_change_color(behaviorViews[AVOIDTRAP], GUI_RED);
						
						return false;
					}
				}
			}
		}
	}
	
	if (active)
	{
		if (activation > cutoff + stopTurning)
		{
			left = right = 18;
			
			if (direction > 0)
				right = direction;	//turn left
			else left = -direction;	//turn right
			
			activation -= decayRate;
			if (activation <= cutoff)
			{
				activation = 0;
				active = false;
				
				directionChange = 0;
				direction = 0;
			}
			
			gui_rectangle_change_size(behaviorViews[AVOIDTRAP], activation, 5);
			gui_rectangle_change_color(behaviorViews[AVOIDTRAP], GUI_GREEN);
			
			return true;
		}
		else
		{
			leftAvoidVisualHeight = rightAvoidVisualHeight = 18;
				
			activation -= decayRate;
			if (activation <= cutoff)
			{
				activation = 0;
				active = false;
				
				directionChange = 0;
				direction = 0;
			}
			
			return false;
		}
	}
	
	activation = 0;
	latentTrapAvoidance = LEFT_AND_RIGHT;
	
	return false;
}

//trys to detect and avoid deadends without having to wander all the way to the end.
//this works by searching for vertical ground edges that may suggest the end of a wall to go around.
//if no such edges are visible, then there is a strong likelyhood of a deadend.
bool Behavior::RunAvoidDeadend(int16 &left, int16 &right)
{
	int16 numMicroVerticalEdges;		//accumulates tiny vertical edges (one pixel tall) in the ground area.  If they stack up on top of each other, this suggests the lack of a deadend.
	bool checkForDeadEnd = false;		//tells when to look for a deadend.
	static int16 mightBeADeadEnd = 0;	//accumulates evidence of a deadend.
	static int16 direction = 0;			//what direction to turn.
	
	if (initializeAllBehaviors)
	{
		activation = 0;
		active = false;
		
		mightBeADeadEnd = 0;
		direction = 0;
		
		return false;
	}
	
	if (groundTop >= 36 && !glanceCounting)
	{
		switch (groundTop)
		{
			case 36:
				if (Random(10) == 0)
				checkForDeadEnd = true;
				break;
			case 37:
				if (Random(9) == 0)
				checkForDeadEnd = true;
				break;
			case 38:
				if (Random(8) == 0)
				checkForDeadEnd = true;
				break;
			case 39:
				if (Random(7) == 0)
				checkForDeadEnd = true;
				break;
			case 40:
				if (Random(6) == 0)
				checkForDeadEnd = true;
				break;
			case 41:
				if (Random(5) == 0)
				checkForDeadEnd = true;
				break;
			case 42:
				if (Random(4) == 0)
				checkForDeadEnd = true;
				break;
			case 43:
				if (Random(3) == 0)
				checkForDeadEnd = true;
				break;
			case 44:
				if (Random(2) == 0)
				checkForDeadEnd = true;
				break;
			default:
				checkForDeadEnd = true;
		}
	}
	
	if (!active || activation == trigger - 8)
	{
		//is the horizon too far away to judge if a deadend if present, break out.
		if (!checkForDeadEnd || groundTop < 36)
		{
			gui_rectangle_change_size(behaviorViews[AVOIDDEADEND], activation, 5);
			gui_rectangle_change_color(behaviorViews[AVOIDDEADEND], GUI_RED);
			
			if (active)
			{
				activation = 0;
				active = false;
				
				mightBeADeadEnd = 0;
				direction = 0;
			}
			
			return false;
		}
		else
		{
			for (int8 x = 0; x < k6300_widthMinus1; x++)
				for (int8 y = groundTop; y < K6300_HEIGHT - 3; y++)
				{
					numMicroVerticalEdges = 0;
					
					for (int8 yy = y; yy < y + 3; yy++)
					{
						if (filteredImageCodes[x][yy] == GROUND && filteredImageCodes[x + 1][yy] != GROUND)
							numMicroVerticalEdges++;
						else yy = y + 3;
					}
					
					if (numMicroVerticalEdges == 3)
					{
						gui_rectangle_change_size(behaviorViews[AVOIDDEADEND], activation, 5);
						gui_rectangle_change_color(behaviorViews[AVOIDDEADEND], GUI_RED);
						
						if (active)
						{
							activation = 0;
							active = false;
							direction = 0;
						}
						
						return false;
					}
					
					numMicroVerticalEdges = 0;
					
					for (int8 yy = y; yy < y + 3; yy++)
					{
						if (filteredImageCodes[x][yy] != GROUND && filteredImageCodes[x + 1][yy] == GROUND)
							numMicroVerticalEdges++;
						else yy = y + 3;
					}
					
					//if there are 3 in a row, there probably is an edge that the robot can go around, and thus there is no deadend.
					if (numMicroVerticalEdges == 3)
					{
						gui_rectangle_change_size(behaviorViews[AVOIDDEADEND], activation, 5);
						gui_rectangle_change_color(behaviorViews[AVOIDDEADEND], GUI_RED);
						
						if (active)
						{
							activation = 0;
							active = false;
							direction = 0;
						}
						
						return false;
					}
				}
			
			if (active)
			{
				activation -= decayRate;
				
				left = direction;
				right = -direction;
				
				gui_rectangle_change_size(behaviorViews[AVOIDDEADEND], activation, 5);
				gui_rectangle_change_color(behaviorViews[AVOIDDEADEND], GUI_GREEN);
				
				return true;
			}
			
			mightBeADeadEnd++;
			
			if (mightBeADeadEnd > 5)
			{
				mightBeADeadEnd = 0;
				activation = trigger + (35 - cpuCost);
				active = true;
				
				if (groundVisible)
				{
					if (difGroundRepulsion > 0)
						direction = 20;
					else direction = -20;
				}
				else
				{
					if (skyBalance < 0)
						direction = -20;
					else direction = 20;
				}
			}
			else return false;
		}
	}
	
	left = direction;
	right = -direction;
	
	activation -= decayRate;
	
	if (activation <= cutoff)
	{
		activation = 0;
		active = false;
		direction = 0;
	}
	if (activation < 0)
		activation = 0;
	
	gui_rectangle_change_size(behaviorViews[AVOIDDEADEND], activation, 5);
	gui_rectangle_change_color(behaviorViews[AVOIDDEADEND], GUI_GREEN);
	
	return true;
}

//without other motivation (such as a visible feeder), this keeps the robot moving forward.
//this also orients the robot towards open floorspace by being attracted to the center of mass of the visible ground.
//similarly, it veers away from the siewalls and towards open sky.
bool Behavior::RunForward(int16 &left, int16 &right)
{
	if (initializeAllBehaviors)
	{
		activation = 0;
		active = false;
		
		return false;
	}
	
	left = 20;
	right = 20;
	
	if (difGroundRepulsion < -50)
		left += difGroundRepulsion / 50;
	else if (difGroundRepulsion > 50)
		right -= difGroundRepulsion / 50;
	
	if (difWallRepulsion < -50)
		left += difWallRepulsion / 50;
	else if (difWallRepulsion > 50)
		right -= difWallRepulsion / 50;
	
	if (skyBalance < -100)
		left += skyBalance / 100;
	else if (skyBalance > 100)
		right -= skyBalance / 100;
		
	if (left < 5)
		left = 5;
	if (right < 5)
		right = 5;
		
	gui_rectangle_change_size(behaviorViews[FORWARD], 100, 5);
	gui_rectangle_change_color(behaviorViews[FORWARD], GUI_BLUE);
	
	return true;
}
//After passing the end of a wall, the robot should glance that direction to see what was hidden by the wall.
//If nothing interesting is in sight this behavior will automatically return the robot back to its original course.
bool Behavior::RunGlanceRight(int16 &left, int16 &right)
{
	static int16 counter = 0;					//keeps robot on course for a while before performing the glance.
	static int32 alphaGroundValue = 0;			//groundValue before glancing.
	static int16 originalLandmarkGoal = BLANK;
	static int16 delayCounter = 25;				//prevents repeated glancing.
	static int16 timer = 0;
	
	if (initializeAllBehaviors || initGlance)
	{
		activation = 0;
		active = false;
		
		counter = 0;
		alphaGroundValue = 0;
		originalLandmarkGoal = BLANK;
		delayCounter = 25;
		timer = 0;
		tryRightGlanceAgain = false;
		glanceCounting = false;
		
		initGlance = false;
		
		return false;
	}
	
	if (killOtherGlance)	//this will trigger the loop below that reinitializes the behavior.
	{
		killOtherGlance = false;
		active = true;
		activation = cutoff;
		timer = 1;
	}
	
	if (active && activation == cutoff + timer && !otherBehaviorPtrs[FEEDERORIENT]->active)	//At 90 degrees check whether to take the new direction.
	{
		if (originalLandmarkGoal == BLANK)										//Since there was no particular attraction in the original direction...
		{
			if (groundValue > alphaGroundValue)									//take the clearer path, or...
			{
				activation = cutoff;
				killOtherGlance = true;
			}
			else if (thePresentTimeStepData.presentLandmarkGoal != BLANK)								//go after any glanced landmarks.
			{
				activation = cutoff;
				killOtherGlance = true;
			}
		}
		else if (!theWorldKnowledge.needsAssociating[originalLandmarkGoal - 8] && thePresentTimeStepData.presentLandmarkGoal != BLANK &&
			(theWorldKnowledge.needsAssociating[thePresentTimeStepData.presentLandmarkGoal - 8] || theWorldKnowledge.timeSinceLastGoal[theImmediateViewKnowledge.visibleLandmarks[thePresentTimeStepData.presentLandmarkGoal] - 8] > 200))
		{
			activation = cutoff;						//Prefer new unassociated landmarks to emphasize learning and going everywhere at least once.
			killOtherGlance = true;						//Also prefer landmarks that haven't been visited in a long time.
		}
	}
	
	if (active && activation > cutoff)
	{
		if (activation == 100)
		{
			delayCounter = 0;
			
			if (cpuCost < 15)
				timer = 21;
			else if (cpuCost < 20)
				timer = 14;
			else if (cpuCost < 25)
				timer = 10;
			else if (cpuCost < 35)
				timer = 8;
			else if (cpuCost < 50)
				timer = 6;
			else if (cpuCost < 80)
				timer = 5;
			else timer = 4;
			
			activation = cutoff + (timer * 2);
			
			if (!otherBehaviorPtrs[LANDMARKORIENT]->active)
				alphaGroundValue = groundValue;
				
			originalLandmarkGoal = thePresentTimeStepData.presentLandmarkGoal;
		}
		
		if (activation > cutoff + timer)
		{
			left = 20;
			right = -20;
		}
		else
		{
			left = -20;
			right = 20;
		}
		
		if (activation == cutoff + timer)
		{
			if (!tryRightGlanceAgain)
				SetNeedToTryAgain(RIGHT);
			else tryRightGlanceAgain = false;
		}
		
		activation -= decayRate;
		if (activation < 0)
			activation = 0;
			
		gui_rectangle_change_size(behaviorViews[GLANCERIGHT], activation, 5);
		gui_rectangle_change_color(behaviorViews[GLANCERIGHT], GUI_GREEN);
		
		return true;
	}
	
	if (active)
	{
		activation = 0;
		counter = 0;
		active = false;
		timer = 0;
	}
	else
	{
		if (!otherBehaviorPtrs[GLANCELEFT]->active)
		{
			if (counter == 0 && FindWallOnRight() && delayCounter >= 65 - cpuCost)
				activation++;
			else if (activation >= trigger && !otherBehaviorPtrs[AVOIDDEADEND]->active)
			{
				counter++;
				
				glanceCounting = true;
				
				//avoidVisualHeight = 8;
				leftAvoidVisualHeight = rightAvoidVisualHeight = 8;
				
				if (IRsensors[2] > 600 || IRsensors[3] > 600 || IRsensors[1] > 450 || IRsensors[4] > 450)
					counter = 999;					//While counting the robot will normally plunge ahead without regard to lower behaviors.
													//This should save it from smashing into a wall.
			}
			else
			{
				activation = 0;
				counter = 0;
			}
		}
		
		if (tryRightGlanceAgain && counter == 0)
		{
			activation = trigger;
			counter = 1;
		}
			
		if (activation > trigger)
			activation = trigger;
		
		//counter makes the robot walk past the end of the wall after the wall is no longer visible peripherally
		if (((cpuCost < 65 && counter >= 80 - cpuCost) || (cpuCost >= 65 && counter >= 10)) && !otherBehaviorPtrs[GLANCELEFT]->active)
		{
			activation = 100;
			active = true;
			counter = 0;
			
			glanceCounting = false;
		}
			
		if (delayCounter < 999)
			delayCounter++;
	}
	
	if (counter == 0)
	{
		gui_rectangle_change_size(behaviorViews[GLANCERIGHT], activation, 5);
		gui_rectangle_change_color(behaviorViews[GLANCERIGHT], GUI_RED);
		
		return false;
	}
	else
	{
		gui_rectangle_change_size(behaviorViews[GLANCERIGHT], activation + counter, 5);
		gui_rectangle_change_color(behaviorViews[GLANCERIGHT], GUI_YELLOW);
		
		return true;
	}
}

//After passing the end of a wall, the robot should glance that direction to see what was hidden by the wall.
//If nothing interesting is in sight this behavior will automatically return the robot back to its original course.
bool Behavior::RunGlanceLeft(int16 &left, int16 &right)
{
	static int16 counter = 0;					//keeps robot on course for a while before performing the glance.
	static int32 alphaGroundValue = 0;			//groundValue before glancing.
	static int16 originalLandmarkGoal = BLANK;
	static int16 delayCounter = 25;				//prevents repeated glancing.
	static int16 timer = 0;
	
	if (initializeAllBehaviors || initGlance)
	{
		activation = 0;
		active = false;
		
		counter = 0;
		alphaGroundValue = 0;
		originalLandmarkGoal = BLANK;
		delayCounter = 25;
		timer = 0;
		tryLeftGlanceAgain = false;
		glanceCounting = false;
		
		return false;
	}
	
	if (killOtherGlance)	//this will trigger the loop below that reinitializes the behavior.
	{
		killOtherGlance = false;
		active = true;
		activation = cutoff;
		timer = 1;
	}
	
	if (active && activation == cutoff + timer && !otherBehaviorPtrs[FEEDERORIENT]->active)	//At 90 degrees check whether to take the new direction.
	{
		if (originalLandmarkGoal == BLANK)										//Since there was no particular attraction in the original direction...
		{
			if (groundValue > alphaGroundValue)									//take the clearer path, or...
			{
				activation = cutoff;
				killOtherGlance = true;
			}
			else if (thePresentTimeStepData.presentLandmarkGoal != BLANK)								//go after any glanced landmarks.
			{
				activation = cutoff;
				killOtherGlance = true;
			}
		}
		else if (!theWorldKnowledge.needsAssociating[originalLandmarkGoal - 8] && thePresentTimeStepData.presentLandmarkGoal != BLANK &&
			(theWorldKnowledge.needsAssociating[thePresentTimeStepData.presentLandmarkGoal - 8] || theWorldKnowledge.timeSinceLastGoal[theImmediateViewKnowledge.visibleLandmarks[thePresentTimeStepData.presentLandmarkGoal] - 8] > 200))
		{
			activation = cutoff;						//Prefer new unassociated landmarks to emphasize learning and going everywhere at least once.
			killOtherGlance = true;						//Also prefer landmarks that haven't been visited in a long time.
		}
	}
	
	if (active && activation > cutoff)
	{
		if (activation == 100)
		{
			delayCounter = 0;
			
			if (cpuCost < 15)
				timer = 21;
			else if (cpuCost < 20)
				timer = 14;
			else if (cpuCost < 25)
				timer = 10;
			else if (cpuCost < 35)
				timer = 8;
			else if (cpuCost < 50)
				timer = 6;
			else if (cpuCost < 80)
				timer = 5;
			else timer = 4;
			
			activation = cutoff + (timer * 2);
			
			if (!otherBehaviorPtrs[LANDMARKORIENT]->active)
				alphaGroundValue = groundValue;
				
			originalLandmarkGoal = thePresentTimeStepData.presentLandmarkGoal;
		}
		
		if (activation > cutoff + timer)
		{
			left = -20;
			right = 20;
		}
		else
		{
			left = 20;
			right = -20;
		}
		
		if (activation == cutoff + timer)
		{
			if (!tryLeftGlanceAgain)
				SetNeedToTryAgain(LEFT);
			else tryLeftGlanceAgain = false;
		}
		
		activation -= decayRate;
		if (activation < 0)
			activation = 0;
			
		gui_rectangle_change_size(behaviorViews[GLANCELEFT], activation, 5);
		gui_rectangle_change_color(behaviorViews[GLANCELEFT], GUI_GREEN);
		
		return true;
	}
	
	if (active)
	{
		activation = 0;
		counter = 0;
		active = false;
	}
	else
	{
		if (!otherBehaviorPtrs[GLANCERIGHT]->active)
		{
			if (counter == 0 && FindWallOnLeft() && delayCounter >= 65 - cpuCost)
				activation++;
			else if (activation >= trigger && !otherBehaviorPtrs[AVOIDDEADEND]->active)
			{
				counter++;
				
				glanceCounting = true;
				
				//avoidVisualHeight = 8;
				leftAvoidVisualHeight = rightAvoidVisualHeight = 8;
				
				if (IRsensors[2] > 400 || IRsensors[3] > 400 || IRsensors[1] > 600 || IRsensors[4] > 600)
					counter = 999;					//While counting the robot will normally plunge ahead without regard to lower behaviors.
													//This should save it from smashing into a wall.
			}
			else
			{
				activation = 0;
				counter = 0;
			}
		}
		
		if (tryLeftGlanceAgain && counter == 0)
		{
			activation = trigger;
			counter = 1;
		}
			
		if (activation > trigger)
			activation = trigger;
		
		//counter makes the robot walk past the end of the wall after the wall is no longer visible peripherally
		if (((cpuCost < 65 && counter >= 80 - cpuCost) || (cpuCost >= 65 && counter >= 10)) && !otherBehaviorPtrs[GLANCERIGHT]->active)
		{
			activation = 100;
			active = true;
			counter = 0;
			
			glanceCounting = false;
		}
			
		if (delayCounter < 999)
			delayCounter++;
	}
	
	if (counter == 0)
	{
		gui_rectangle_change_size(behaviorViews[GLANCELEFT], activation, 5);
		gui_rectangle_change_color(behaviorViews[GLANCELEFT], GUI_RED);
		
		return false;
	}
	else
	{
		gui_rectangle_change_size(behaviorViews[GLANCELEFT], activation + counter, 5);
		gui_rectangle_change_color(behaviorViews[GLANCELEFT], GUI_YELLOW);
		
		return true;
	}
}

//This executes a 360 degree scan and then releases control of the robot to the other behaviors.
//If, during the scan, interesting features are detected such as feeders or landmarks,
//the appropriate orienting behaviors will take control of the robot and turn this off.
bool Behavior::RunScanSearch(int16 &left, int16 &right)
{
	static int16 direction = 0;			//what direction = turn
	
	if (initializeAllBehaviors)
	{
		activation = 0;
		active = false;
		
		direction = 0;
		turnOnScanSearchLeft = turnOnScanSearchRight = false;
		
		return false;
	}
	
	if (turnOnScanSearchLeft || turnOnScanSearchRight)
	{
		if (turnOnScanSearchLeft)
		{
			direction = -20;
			turnOnScanSearchLeft = false;
		}
		else
		{
			direction = 20;
			turnOnScanSearchRight = false;
		}
		active = false;
		activation = trigger;
	}
	
	if (active && activation > cutoff)
	{
		left = direction;
		right = -direction;
		
		activation -= decayRate;
		if (activation < 0)
			activation = 0;
		
		if (activation == cutoff)
		{
			seekingFeeder = false;
			seekingCloseFeeder = false;
		}
			
		gui_rectangle_change_size(behaviorViews[SCANSEARCH], activation, 5);
		gui_rectangle_change_color(behaviorViews[SCANSEARCH], GUI_GREEN);
		
		return true;
	}
	else if (!active && activation >= trigger && !otherBehaviorPtrs[GLANCERIGHT]->active && !otherBehaviorPtrs[GLANCELEFT]->active)
	{
		active = true;
		
		initGlance = true;
		
		left = direction;
		right = -direction;
		
		if (cpuCost < 10)
			activation = 110;
		else if (cpuCost < 15)
			activation = 80;
		else if (cpuCost < 20)
			activation = 60;
		else if (cpuCost < 25)
			activation = 48;
		else if (cpuCost < 30)
			activation = 40;
		else if (cpuCost < 35)
			activation = 33;
		else if (cpuCost < 40)
			activation = 28;
		else if (cpuCost < 45)
			activation = 24;
		else if (cpuCost < 50)
			activation = 21;
		else if (cpuCost < 55)
			activation = 19;
		else if (cpuCost < 60)
			activation = 17;
		else if (cpuCost < 65)
			activation = 15;
		else if (cpuCost < 70)
			activation = 14;
		else if (cpuCost < 75)
			activation = 13;
		else if (cpuCost < 80)
			activation = 12;
		else activation = 11;
		
		activation -= decayRate;
		if (activation < 0)
			activation = 0;
			
		gui_rectangle_change_size(behaviorViews[SCANSEARCH], activation, 5);
		gui_rectangle_change_color(behaviorViews[SCANSEARCH], GUI_GREEN);
		
		return true;
	}
	
	activation = 0;
	turnOnScanSearchLeft = turnOnScanSearchRight = false;
	
	active = false;
	direction = 0;
	seekingFeeder = false;
	seekingCloseFeeder = false;
	
	gui_rectangle_change_size(behaviorViews[SCANSEARCH], activation, 5);
	gui_rectangle_change_color(behaviorViews[SCANSEARCH], GUI_RED);
	
	return false;
}

//orients towards the chosen landmark goal
bool Behavior::RunLandmarkOrient(int16 &left, int16 &right)
{
	uint8 timer;									//how long must the robot go after a landmark before a 360 scan is attempted to find a green feeder?
	static int16 direction = 0;						//what direction to search for a lost landmark (presumably off the edge of the camera view).
	static int16 timeSpentGoingAfterLandmark = 0;	//how long has the robot been going after the landmark?  Used to trigger an occasional scan.
	
	if (initializeAllBehaviors)
	{
		activation = 0;
		active = false;
		
		direction = 0;
		
		return false;
	}
	
	active = false;
	
	if (!turnOnScanSearchLeft && !turnOnScanSearchRight && thePresentTimeStepData.presentLandmarkGoal != BLANK && !seekingFeeder && !seekingCloseFeeder)
	{
		int16 landmarkCenterOffset = LookForLandmarkHorizCenter();
		
		left = right = 20;
		
		if (landmarkCenterOffset != 9999)
		{
			timeSpentGoingAfterLandmark++;
			
			if (cpuCost < 15)
				timer = 150;
			else if (cpuCost < 25)
				timer = 110;
			else if (cpuCost < 35)
				timer = 80;
			else if (cpuCost < 45)
				timer = 65;
			else timer = 50;
			
			//consider doing an occasional scan for a green feeder.
			if (theImmediateViewKnowledge.landmarkDistances[thePresentTimeStepData.presentLandmarkGoalMemoryIndex] > 5)
				if (timeSpentGoingAfterLandmark >= timer && IRsensors[0] < 200 && IRsensors[1] < 200 && IRsensors[2] < 200
														 && IRsensors[3] < 200 && IRsensors[4] < 200 && IRsensors[5] < 200)
				{
					turnOnScanSearchLeft = true;
					
					seekingFeeder = true;
					
					activation = 0;
					active = false;
					
					timeSpentGoingAfterLandmark = 0;
					
					return false;	//Do a an occasional spin to search for a green feeder
				}
				
			//since landmarkCenterOffset can be in the range of -40 - 40
			//we will scale this to a value between -20 - 20.
			landmarkCenterOffset /= 2;
			
			if (landmarkCenterOffset < 0)
			{
				left += landmarkCenterOffset;
				direction = 1;
				leftAvoidVisualHeight = 16 - (20 - left);
				if (leftAvoidVisualHeight < 4)
					leftAvoidVisualHeight = 4;
			}
			else
			{
				right -= landmarkCenterOffset;
				direction = 0;
				rightAvoidVisualHeight = 16 - (20 - right);
				if (rightAvoidVisualHeight < 4)
					rightAvoidVisualHeight = 4;
			}
			
			activation = trigger;
			
			active = true;
			
			gui_rectangle_change_size(behaviorViews[LANDMARKORIENT], 100, 5);
			gui_rectangle_change_color(behaviorViews[LANDMARKORIENT], GUI_BLUE);
			
			return true;
		}
	}
	else if (activation > cutoff)
	{
		if (direction == 0)
			right = 0;
		else left = 0;
		
		activation -= decayRate;
		if (activation < 0)
			activation = 0;
		
		active = true;
		
		//avoidVisualHeight = 8;
		leftAvoidVisualHeight = rightAvoidVisualHeight = 4;
		
		gui_rectangle_change_size(behaviorViews[LANDMARKORIENT], 100, 5);
		gui_rectangle_change_color(behaviorViews[LANDMARKORIENT], GUI_YELLOW);
		
		return true;
	}
	
	timeSpentGoingAfterLandmark = 0;
	
	return false;
}

//orients towards a visible feeder
bool Behavior::RunFeederOrient(int16 &left, int16 &right)
{
	int16 feederCenterOffset = 0;					//where is the center of the feeder in the camera (how sharply should the robot turn towards it)?
	uint8 timer;									//how long must the robot go after a feeder box before a 360 scan is attempted to find a green feeder?
	static int16 predictedOffset = 0;				//used to try to find a feeder tha has disappeared (presumably off the edge of the camera view).
	static int16 stayOnCourse = 0;					//holds the robot going straight for a short period (so it can get in front of the feeder).
	static int16 stayOnCourseDuration = 4;			//how long to hold on course.
	static bool goalIsAFeederBox = false;			//is the feeder a green feeder or just a feeder box?
	static int16 timeSpentGoingAfterFeederBox = 0;	//how long has the robot been going after the feeder box?  Used to trigger an occasional scan.
	
	if (initializeAllBehaviors || initFeederOrient)
	{
		activation = 0;
		active = false;
		
		predictedOffset = 0;
		stayOnCourse = 0;
		stayOnCourseDuration = 4;
		goalIsAFeederBox = false;
		timeSpentGoingAfterFeederBox = 0;
		
		initFeederOrient = false;
		
		return false;
	}
	
	//should the robot hold steady to get in front of the feeder?
	if (stayOnCourse == 0 && feederOnDetected)
	{
		if (feederOnBound.left == 0 && feederOnBound.height > 30)
		{
			stayOnCourse = -1;
			stayOnCourseDuration += 8;
			activation = cutoff + stayOnCourseDuration;
		}
		if (feederOnBound.right == k6300_widthMinus1 && feederOnBound.height > 30)
		{
			stayOnCourse = 1;
			stayOnCourseDuration += 8;
			activation = cutoff + stayOnCourseDuration;
		}
	}
	
	//hold steady to get in front of the feeder.
	if (stayOnCourse != 0)
	{
		left = right = 20;
		
		active = true;
		
		activation -= decayRate;
		if (activation <= cutoff)
		{
			if (stayOnCourse == -1)			//Do a scan to find the feeder, which should be red by now.
				turnOnScanSearchLeft = true;
			else turnOnScanSearchRight = true;
			
			seekingCloseFeeder = true;
			
			activation = 0;
			stayOnCourse = 0;
			predictedOffset = 0;
			active = false;
		}
		
		recentlyFeederOrienting = 10;
		
		stayOnCourseDuration = 4;
		
		gui_rectangle_change_size(behaviorViews[FEEDERORIENT], 100, 5);
		gui_rectangle_change_color(behaviorViews[FEEDERORIENT], GUI_BLUE);
		
		return true;
	}
	
	//if scanning exclusively for a green feeder and one isn't detected, break out.
	if (!feederOnDetected && otherBehaviorPtrs[SCANSEARCH]->active)
		return false;
	
	feederCenterOffset = LookForFeederHorizCenter();
	
	left = right = 20;
	
	if (recentlyFeederOrienting > 0 && !feederOnDetected)
		feederCenterOffset = 9999;
	
	if (feederCenterOffset < 8888)	//A feeder is presently visible
	{
		if (!feederOnDetected)
		{
			goalIsAFeederBox = true;
			
			timeSpentGoingAfterFeederBox++;
			
			if (cpuCost < 15)
				timer = 150;
			else if (cpuCost < 25)
				timer = 110;
			else if (cpuCost < 35)
				timer = 80;
			else if (cpuCost < 45)
				timer = 65;
			else timer = 50;
			
			//if going after a feeder box, do an occasional scan for a green feeder.
			if (feederOnDistance > 5)
				if (timeSpentGoingAfterFeederBox >= timer && IRsensors[0] < 200 && IRsensors[1] < 200 && IRsensors[2] < 200
														 && IRsensors[3] < 200 && IRsensors[4] < 200 && IRsensors[5] < 200)
				{
					turnOnScanSearchLeft = true;
					
					goalIsAFeederBox = false;
					
					seekingFeeder = true;
					
					predictedOffset = 0;
					
					activation = 0;
					active = false;
					
					stayOnCourseDuration = 4;
					
					timeSpentGoingAfterFeederBox = 0;
					
					return false;	//Do a an occasional spin to search for a green feeder.
				}
		}
		else
		{
			timeSpentGoingAfterFeederBox = 0;
			goalIsAFeederBox = false;
		}
		
		seekingFeeder = false;
		seekingCloseFeeder = false;
		stayOnCourse = 0;
		
		//since feederCenterOffset can be in the range of -40 - 40
		//we will scale this to a value between -20 - 20.
		feederCenterOffset /= 2;
		
		if (feederCenterOffset < 0 && stayOnCourse != 1)
		{
			left += feederCenterOffset;
			leftAvoidVisualHeight = 16 - (20 - left);
			if (leftAvoidVisualHeight < 4)
				leftAvoidVisualHeight = 4;
		}
		else if (feederCenterOffset > 0 && stayOnCourse != -1)
		{
			right -= feederCenterOffset;
			rightAvoidVisualHeight = 16 - (20 - right);
			if (rightAvoidVisualHeight < 4)
				rightAvoidVisualHeight = 4;
		}
		
		predictedOffset = feederCenterOffset / 2;
		
		activation = trigger;
		
		if (!goalIsAFeederBox)
		{
			if (cpuCost < 20)
				activation += 50;
			else if (cpuCost < 25)
				activation += 35;
			else if (cpuCost < 30)
				activation += 20;
			else if (cpuCost < 35)
				activation += 10;
			else if (cpuCost < 50)
				activation += 5;
			else if (cpuCost < 80)
				activation += 0;
			else activation -= 5;
		}
		else
		{
			if (cpuCost < 20)
				activation += 75;
			else if (cpuCost < 25)
				activation += 65;
			else if (cpuCost < 30)
				activation += 50;
			else if (cpuCost < 35)
				activation += 35;
			else if (cpuCost < 50)
				activation += 25;
			else if (cpuCost < 80)
				activation += 15;
			else activation += 10;
		}
		
		active = true;
		
		if (!goalIsAFeederBox)
			recentlyFeederOrienting = activation + 1;
		
		//avoidVisualHeight = 12;
		
		stayOnCourseDuration = 4;
		
		gui_rectangle_change_size(behaviorViews[FEEDERORIENT], 100, 5);
		gui_rectangle_change_color(behaviorViews[FEEDERORIENT], GUI_BLUE);
		
		return true;
	}
	else if (activation > cutoff)	//if it lost sight of the feeder it will still try to find the right direction
	{
		stayOnCourse = 0;
		
		if (feederCenterOffset == 8888)
			activation = 0;	//The robot probably just fed, so do a postFeedScanAssociate.
		
		if (predictedOffset < 0)
			left += predictedOffset;
		else right -= predictedOffset;
		
		activation -= decayRate;
		if (activation < 0)
		{
			activation = 0;
			stayOnCourse = 0;
		}
		
		active = true;
		
		if (!goalIsAFeederBox)
			recentlyFeederOrienting = activation + 1;
		
		stayOnCourseDuration = 4;
		
		//avoidVisualHeight = 8;
		leftAvoidVisualHeight = rightAvoidVisualHeight = 4;
		
		gui_rectangle_change_size(behaviorViews[FEEDERORIENT], 100, 5);
		gui_rectangle_change_color(behaviorViews[FEEDERORIENT], GUI_YELLOW);
		
		return true;
	}
	else if (activation == cutoff)	//no feeder is visible and the latent period has run out, so do a scanSearch.
	{
		stayOnCourse = 0;
		
		if (predictedOffset < 0)			//Do a scan to find the missing feeder
			turnOnScanSearchLeft = true;
		else turnOnScanSearchRight = true;
		
		goalIsAFeederBox = false;
		
		seekingFeeder = true;
		
		predictedOffset = 0;
		
		activation = 0;
		active = false;
		
		stayOnCourseDuration = 4;
		
		timeSpentGoingAfterFeederBox = 0;
		
		return false;	//No feeder is in sight and no recent memory is available to direct feederOrient
	}
	
	stayOnCourse = 0;
		
	stayOnCourseDuration = 4;
	timeSpentGoingAfterFeederBox = 0;
	
	return false;
}

//Executes a 360 degree scan during which the robot will note all visible landmarks.
//Since this behavior is triggered from a present landmark, this creates memory associations of
//visible landmarks from the present landmark, helping to build a landmark-based map of the world.
bool Behavior::RunScanAssociate(int16 &left, int16 &right)
{
	static int16 timerSet = 0;
	static bool startLookingForPresentLandmark = false;
	static bool turning = false;
	
	if (initializeAllBehaviors)
	{
		activation = 0;
		active = false;
		
		timerSet = 0;
		startLookingForPresentLandmark = false;
		turning = false;
		
		return false;
	}
	
	if (!active && !otherBehaviorPtrs[FEEDERORIENT]->active && thePresentTimeStepData.presentLandmark != -1 && theWorldKnowledge.needsAssociating[thePresentTimeStepData.presentLandmark - 8])
	{
		activation = trigger;
		active = true;
		
		thePresentTimeStepData.presentLandmarkGoal = BLANK;
		thePresentTimeStepData.presentLandmarkGoalBox = zeroDimBox;
		
		theWorldKnowledge.needsAssociating[thePresentTimeStepData.presentLandmark - 8] = false;
	}
		
	if (active)
	{
		if (thePresentTimeStepData.presentLandmarkOutOfView)
			startLookingForPresentLandmark = true;
		
		if (!startLookingForPresentLandmark)
		{	
			left = -20;
			right = 20;
			
		}
		else if (thePresentTimeStepData.presentLandmarkOutOfView && !turning)	//must be looking for thePresentTimeStepData.presentLandmark to end one full spin.  Also not trying to turn around.
		{
			left = -20;
			right = 20;
		}
		//else if (!turnOnScanSearchLeft && !turnOnScanSearchRight)	//thePresentTimeStepData.presentLandmark must be set and the robot is turning around
		else if (postScanTurnAmount == 0)	//thePresentTimeStepData.presentLandmark must be seen and the robot couldn't find anything to go after, so it is turning around.
		{
			if (timerSet == 0)
			{
				activation = cutoff + ((trigger - activation) / 2) + 1;
				timerSet = 1;
				turning = true;
			}
				
			left = -20;
			right = 20;
		}
		else	//robot will now turn towards the most interesting object spotted.
		{
			if (timerSet == 0)
			{
				if (postScanTurnAmount < 0)
					activation = cutoff + (-postScanTurnAmount);
				else activation = cutoff + postScanTurnAmount;
				
				timerSet = postScanTurnAmount;
				turning = true;
			}
			
			if (feederOnDetected && feederOnBound.height >= desiredFeederHeight)	//Override the turn if a desired feeder is spotted.
			{
				active = false;
				
				gui_rectangle_change_size(behaviorViews[SCANASSOCIATE], activation, 5);
				gui_rectangle_change_color(behaviorViews[SCANASSOCIATE], GUI_RED);
				
				return false;
			}
			else
			{
				if (timerSet < 0)
				{
					left = -20;
					right = 20;
				}
				else
				{
					left = 20;
					right = -20;
				}
			}
		}
		
		activation -= decayRate;
		if (activation < 0)
			activation = 0;
			
		if (activation <= cutoff)
		{
			active = false;
			activation = 0;
		}
		
		initializeAllBehaviors = true;
			
		gui_rectangle_change_size(behaviorViews[SCANASSOCIATE], activation, 5);
		gui_rectangle_change_color(behaviorViews[SCANASSOCIATE], GUI_GREEN);
		
		return true;
	}
	
	activation = 0;
	
	active = false;
	timerSet = 0;
	startLookingForPresentLandmark = false;
	turning = false;
	
	gui_rectangle_change_size(behaviorViews[SCANASSOCIATE], activation, 5);
	gui_rectangle_change_color(behaviorViews[SCANASSOCIATE], GUI_RED);
	
	return false;
}

bool Behavior::RunPostFeedScanAssociate(int16 &left, int16 &right)
{
	static int16 delayCounter = 50;
	static int16 timerSet = 0;
	static bool startLookingForFeederOff = false;
	static bool turning = false;
	
	if (initializeAllBehaviors)
	{
		activation = 0;
		active = false;
		
		delayCounter = 50;
		timerSet = 0;
		startLookingForFeederOff = false;
		turning = false;
		
		return false;
	}
	
	if (nearFeederOff && !active && recentlyFeederOrienting > 0)
		activation = trigger;
		
	if (active)
	{
		if (!nearFeederOff)
			startLookingForFeederOff = true;
		
		if (!startLookingForFeederOff)
		{	
			left = -20;
			right = 20;
		}
		else if (!nearFeederOff && !turning)	//must be looking for nearFeederOff to end one full spin.  Also not trying to turn around.
		{
			left = -20;
			right = 20;
		}
		//else if (!turnOnScanSearchLeft && !turnOnScanSearchRight)	//nearFeederOff must be true and the robot is turning around.
		else if (postScanTurnAmount == 0)	//nearFeederOff must be true and the robot couldn't find anything to go after, so it is turning around.
		{
			if (timerSet == 0)
			{
				activation = cutoff + ((trigger - activation) / 2) + 1;
				timerSet = 1;
				turning = true;
			}
				
			left = -20;
			right = 20;
		}
		else	//robot will now turn towards the most interesting object spotted.
		{
			if (timerSet == 0)
			{
				if (postScanTurnAmount < 0)
					activation = cutoff + (-postScanTurnAmount);
				else activation = cutoff + postScanTurnAmount;
				
				timerSet = postScanTurnAmount;
				turning = true;
			}
			
			if (feederOnDetected && feederOnBound.height >= desiredFeederHeight)	//Override the turn if a desired feeder is spotted.
			{
				active = false;
				
				gui_rectangle_change_size(behaviorViews[SCANASSOCIATE], activation, 5);
				gui_rectangle_change_color(behaviorViews[SCANASSOCIATE], GUI_RED);
				
				return false;
			}
			else
			{
				if (timerSet < 0)
				{
					left = -20;
					right = 20;
				}
				else
				{
					left = 20;
					right = -20;
				}
			}
		}
		//else activation = cutoff;
		
		activation -= decayRate;
		if (activation < 0)
			activation = 0;
		
		if (activation <= cutoff)
		{
			activation = 0;
			active = false;
			timerSet = 0;
			startLookingForFeederOff = false;
			turning = false;
		}
		
		if (cpuCost <= 70)
			recentlyFed = 150 - cpuCost;
		else recentlyFed = 80;
		
		initializeAllBehaviors = true;
			
		gui_rectangle_change_size(behaviorViews[POSTFEEDSCANASSOCIATE], activation, 5);
		gui_rectangle_change_color(behaviorViews[POSTFEEDSCANASSOCIATE], GUI_GREEN);
		
		return true;
	}
	else if (!active && activation >= trigger && delayCounter >= 50)
	{
		active = true;
		
		left = -20;
		right = 20;
		
		activation -= decayRate;
		if (activation < 0)
			activation = 0;
			
		delayCounter = 0;
		
		if (cpuCost <= 70)
			recentlyFed = 150 - cpuCost;
		else recentlyFed = 80;
		
		recentlyFeederOrienting = 0;
		
		initializeAllBehaviors = true;
			
		gui_rectangle_change_size(behaviorViews[POSTFEEDSCANASSOCIATE], activation, 5);
		gui_rectangle_change_color(behaviorViews[POSTFEEDSCANASSOCIATE], GUI_GREEN);
		
		return true;
	}
	
	if (delayCounter < 50)
		delayCounter++;
	
	activation = 0;
	
	active = false;
	timerSet = 0;
	startLookingForFeederOff = false;
	turning = false;
	
	gui_rectangle_change_size(behaviorViews[POSTFEEDSCANASSOCIATE], activation, 5);
	gui_rectangle_change_color(behaviorViews[POSTFEEDSCANASSOCIATE], GUI_RED);
	
	return false;
}

//constant front IR activity suggests the robot is trying to get through an obstacle.
//This might occur if the robot can see a feeder past the obstacle creating the interference.
bool Behavior::RunEscapeCorner(int16 &left, int16 &right)
{
	int16 numSensorsHit = 0;						//how many front IR sensors are registering an object?
	static int16 direction = 0;						//what direction to turn.
	static int16 leftStuck = 0;						//is the left side stuck?
	static int16 rightStuck = 0;					//is the right side stuck?
	static int8 startTurning = 0;					//when should the robot stop backing up and start turning?
	static int16 timeSinceLastCornerEscape = 65;	//used to determine if repeated escape attempts are being made.
	
	if (initializeAllBehaviors)
	{
		activation = 0;
		active = false;
		
		direction = 0;
		leftStuck = 0;
		rightStuck = 0;
		startTurning = 0;
		timeSinceLastCornerEscape = 65;
		
		return false;
	}
	
	if (!active)
	{
		timeSinceLastCornerEscape++;
		
		//how many front sensors are registering a hit?
		for (int8 i = 1; i <= 4; i++)
			if (IRsensors[i] > 15)
				numSensorsHit++;
		
		if (numSensorsHit > 2)
			activation += 2;
		else if (numSensorsHit == 2)
			activation++;
		else
		{
			activation -= (2 - numSensorsHit);
			if (activation < 0)
				activation = 0;
		}
		
		if (IRsensors[0] > 800)
			leftStuck += 2;
		else if (IRsensors[0] > 500)
			leftStuck++;
		else leftStuck--;
		
		if (IRsensors[5] > 800)
			rightStuck += 2;
		else if (IRsensors[5] > 500)
			rightStuck++;
		else rightStuck--;
		
		if (leftStuck < 0)
			leftStuck = 0;
		if (rightStuck < 0)
			rightStuck = 0;
		
		if (leftStuck > 80 - cpuCost)
			activation = 999;
		else if (rightStuck > 80 - cpuCost)
			activation = 999;
	}
	
	if (otherBehaviorPtrs[SCANSEARCH]->active || otherBehaviorPtrs[SCANASSOCIATE]->active || otherBehaviorPtrs[POSTFEEDSCANASSOCIATE]->active)
		activation = 0;
	
	if (!active && activation >= trigger + (40 - cpuCost) && !otherBehaviorPtrs[SCANSEARCH]->active && !otherBehaviorPtrs[SCANASSOCIATE]->active && !otherBehaviorPtrs[POSTFEEDSCANASSOCIATE]->active)
	{
		activation = 15;
		
		if (cpuCost < 20)
		{
			activation = 45;
			startTurning = 10;
		}
		else if (cpuCost < 30)
		{
			activation = 35;
			startTurning = 7;
		}
		else if (cpuCost < 40)
		{
			activation = 25;
			startTurning = 4;
		}
		else if (cpuCost < 60)
		{
			activation = 20;
			startTurning = 3;
		}
		else if (cpuCost < 80)
		{
			activation = 15;
			startTurning = 2;
		}
		else
		{
			activation = 10;
			startTurning = 1;
		}
		
		//if an escape attempt was made recently, this one should be more drastic in its attempt to escape by turning away more.
		if (timeSinceLastCornerEscape < 120 - cpuCost)
		{
			activation += 5;
			startTurning += 5;
		}
		
		timeSinceLastCornerEscape = 0;
		
		active = true;
	}
	
	if (active)
	{
		if (activation > cutoff + startTurning)
		{
			left = -20;
			right = -20;
			
			if (IRsensors[6] > 500 || IRsensors[7] > 500)
				activation = cutoff + startTurning + 1;
		}
		else if (activation == cutoff + startTurning)
		{
			if (difGroundRepulsion < 0)
				direction = -20;
			else direction = 20;
		}
		else
		{
			left = direction;
			right = -direction;
		}
		
		activation -= decayRate;
		if (activation < 0)
			activation = 0;
		
		if (activation <= cutoff)
		{
			activation = 0;
			active = false;
			direction = 0;
			leftStuck = 0;
			rightStuck = 0;
			startTurning = 0;
			timeSinceLastCornerEscape = 0;
		}
	
		gui_rectangle_change_size(behaviorViews[ESCAPECORNER], activation, 5);
		gui_rectangle_change_color(behaviorViews[ESCAPECORNER], GUI_GREEN);
		
		return true;
	}
	
	if (activation > 0)
	{
		gui_rectangle_change_size(behaviorViews[ESCAPECORNER], activation, 5);
		gui_rectangle_change_color(behaviorViews[ESCAPECORNER], GUI_RED);
	}
	else if (timeSinceLastCornerEscape >= 120 - cpuCost)
	{
		gui_rectangle_change_size(behaviorViews[ESCAPECORNER], 120 - cpuCost, 5);
		gui_rectangle_change_color(behaviorViews[ESCAPECORNER], GUI_YELLOW);
	}
	else
	{
		gui_rectangle_change_size(behaviorViews[ESCAPECORNER], timeSinceLastCornerEscape, 5);
		gui_rectangle_change_color(behaviorViews[ESCAPECORNER], GUI_CYAN);
	}
	
	return false;
}

//a few identical frames in a row means the robot is stuck
bool Behavior::RunEscapeStuck(int16 &left, int16 &right)
{
	int16 veryOldDifferenceAccumulator = 0;							//accumulates differences between an old camera image and the present camera image.
	int16 modifier = 0;												//lowers trigger point if side IR activity is present.
	static uint8 veryOldFilteredImage[K6300_WIDTH][K6300_HEIGHT];	//the old image.
	static int16 veryOldCounter = 50;								//how often to check images for similarity.
	static int16 direction = 0;										//does the robot back up or turn?  If turn, which direction?
	static int16 timeSinceLastStuckEscape = 65;						//used to determine if repeated escape attempts are being made.
	static int8 turnAmount = -15;									//how sharply to turn.
	
	if (!active)
	{
		timeSinceLastStuckEscape++;
		
		veryOldCounter++;
		if (veryOldCounter >= 110 - cpuCost)
		{
			veryOldCounter = 0;
			
			for (int8 y = 0; y < K6300_HEIGHT; y++)
				for (int8 x = 0; x < K6300_WIDTH; x++)
				{
					if (filteredImageCodes[x][y] != veryOldFilteredImage[x][y])
						veryOldDifferenceAccumulator++;
					
					veryOldFilteredImage[x][y] = filteredImageCodes[x][y];
				}
			
			if (IRsensors[0] >= 600 || IRsensors[1] >= 600)
			{
				modifier = 100;
				
				direction = 1;
			}
			else if (IRsensors[5] >= 600 || IRsensors[4] >= 600)
			{
				modifier = 100;
				
				direction = -1;
			}
			else direction = 0;
				
			if (veryOldDifferenceAccumulator < 200 + modifier)
				activation = trigger;
		}
		
		if (activation < 0)
			activation = 0;
	}
	
	if (((!active && activation >= trigger) || triggerEscapeStuck) && !otherBehaviorPtrs[SCANSEARCH]->active && !otherBehaviorPtrs[SCANASSOCIATE]->active && !otherBehaviorPtrs[POSTFEEDSCANASSOCIATE]->active)
	{
		activation = cutoff + 8;
		active = true;
		triggerEscapeStuck = false;
		
		//if an escape attempt was made recently, this one should be more drastic in its attempt to escape by turning away more.
		if (timeSinceLastStuckEscape < 120 - cpuCost)
			turnAmount += 5;
		else turnAmount = -15;
		
		timeSinceLastStuckEscape = 0;
	}
	
	if (active && activation > cutoff)
	{
		switch (direction)
		{
			case -1:
				left = -20;
				right = 20;
				break;
			case 0:
				left = right = -20;
				break;
			case 1:
				left = 20;
				right = -20;
				break;
		}
		
		if (direction == 0 && activation < cutoff + (300 / cpuCost))
		{
			if (difGroundRepulsion < 0)
				right = turnAmount;		//turn left
			else left = turnAmount;		//turn right
		}
		
		activation -= decayRate;
		if (activation < 0)
			activation = 0;
		
		if (activation <= cutoff || (direction == 0 && (IRsensors[6] > 500 || IRsensors[7] > 500)))
		{
			activation = 0;
			active = false;
			direction = 0;
		}
		
		initializeAllBehaviors = true;
	
		gui_rectangle_change_size(behaviorViews[ESCAPESTUCK], activation, 5);
		gui_rectangle_change_color(behaviorViews[ESCAPESTUCK], GUI_GREEN);
		
		return true;
	}
	
	if (timeSinceLastStuckEscape >= 120 - cpuCost)
	{
		gui_rectangle_change_size(behaviorViews[ESCAPESTUCK], 120 - cpuCost, 5);
		gui_rectangle_change_color(behaviorViews[ESCAPESTUCK], GUI_YELLOW);
	}
	else
	{
		gui_rectangle_change_size(behaviorViews[ESCAPESTUCK], timeSinceLastStuckEscape, 5);
		gui_rectangle_change_color(behaviorViews[ESCAPESTUCK], GUI_CYAN);
	}
	
	return false;
}

//  BEHAVIOR  *******************************************************
//  MOOD  *******************************************************
//The whole mood issue is pretty obsolete at this point.  The lowerbehaviors take care of everything that the different moods used to do.
//The robot is constantly in the SEARCHING mood as a result.  There used to be other moods that would use different behaviors in a different
//hierarchy.  Strictly speaking, I suppose I could eliminate all the intermediary Mood related code, but it probably isn't worth the trouble,
//and since the concept is sound, I may want to reintroduce different moods at some future time.
#define SEARCHING	0

class Mood
{
	private:
		int16 left, right;	//motor commands
		
	public:
		Behavior theBehaviors[NUMBEHAVIORS];
		
		Mood();
		~Mood();
		void Copy(Mood &m);
		void Initialize(int16 whichMood);
		
		int16 RunMood(int16 &theLeft, int16 &theRight);
};

Mood::Mood()
{
}

Mood::~Mood()
{
}

void Mood::Copy(Mood &m)
{
	left = m.left;
	right = m.right;
	
	for (int8 i = 0; i < NUMBEHAVIORS; i++)
		theBehaviors[i] = m.theBehaviors[i];
}

void Mood::Initialize(int16 whichMood)
{
	Behavior *ptrs[NUMBEHAVIORS];
	for (int8 i = 0; i < NUMBEHAVIORS; i++)
		ptrs[i] = &theBehaviors[i];
	
	bool theInhibits[NUMBEHAVIORS];
	
	for (int8 i = 0; i < NUMBEHAVIORS; i++)
		theInhibits[i] = false;
	
	switch (whichMood)
	{
		case SEARCHING:
			theInhibits[AVOID_IR]			= false;
			theInhibits[AVOIDVISUAL]		= false;
			theInhibits[AVOIDTRAP]			= false;
			theInhibits[AVOIDDEADEND]		= false;
			theInhibits[FORWARD]			= false;
			theInhibits[GLANCERIGHT]		= false;
			theInhibits[GLANCELEFT]			= false;
			theInhibits[SCANSEARCH]			= false;
			theInhibits[LANDMARKORIENT]		= false;
			theInhibits[FEEDERORIENT]		= false;
			theInhibits[SCANASSOCIATE]		= false;
			theInhibits[POSTFEEDSCANASSOCIATE]		= false;
			theInhibits[ESCAPECORNER]		= false;
			theInhibits[ESCAPESTUCK]		= false;
			theBehaviors[AVOID_IR].Initialize			(AVOID_IR, 0, 0, 0, theInhibits, ptrs);
			theInhibits[AVOID_IR]			= false;
			theInhibits[AVOIDVISUAL]		= false;
			theInhibits[AVOIDTRAP]			= false;
			theInhibits[AVOIDDEADEND]		= false;
			theInhibits[FORWARD]			= false;
			theInhibits[GLANCERIGHT]		= false;
			theInhibits[GLANCELEFT]			= false;
			theInhibits[SCANSEARCH]			= false;
			theInhibits[LANDMARKORIENT]		= false;
			theInhibits[FEEDERORIENT]		= false;
			theInhibits[SCANASSOCIATE]		= false;
			theInhibits[POSTFEEDSCANASSOCIATE]		= false;
			theInhibits[ESCAPECORNER]		= false;
			theInhibits[ESCAPESTUCK]		= false;
			theBehaviors[AVOIDVISUAL].Initialize		(AVOIDVISUAL, 0, 0, 0, theInhibits, ptrs);
			theInhibits[AVOID_IR]			= false;
			theInhibits[AVOIDVISUAL]		= true;
			theInhibits[AVOIDTRAP]			= false;
			theInhibits[AVOIDDEADEND]		= false;
			theInhibits[FORWARD]			= false;
			theInhibits[GLANCERIGHT]		= false;
			theInhibits[GLANCELEFT]			= false;
			theInhibits[SCANSEARCH]			= false;
			theInhibits[LANDMARKORIENT]		= false;
			theInhibits[FEEDERORIENT]		= false;
			theInhibits[SCANASSOCIATE]		= false;
			theInhibits[POSTFEEDSCANASSOCIATE]		= false;
			theInhibits[ESCAPECORNER]		= false;
			theInhibits[ESCAPESTUCK]		= false;
			theBehaviors[AVOIDTRAP].Initialize		(AVOIDTRAP, 4, 1, 1, theInhibits, ptrs);
			theInhibits[AVOID_IR]			= true;
			theInhibits[AVOIDVISUAL]		= true;
			theInhibits[AVOIDTRAP]			= true;
			theInhibits[AVOIDDEADEND]		= false;
			theInhibits[FORWARD]			= false;
			theInhibits[GLANCERIGHT]		= false;
			theInhibits[GLANCELEFT]			= false;
			theInhibits[SCANSEARCH]			= false;
			theInhibits[LANDMARKORIENT]		= false;
			theInhibits[FEEDERORIENT]		= false;
			theInhibits[SCANASSOCIATE]		= false;
			theInhibits[POSTFEEDSCANASSOCIATE]		= false;
			theInhibits[ESCAPECORNER]		= false;
			theInhibits[ESCAPESTUCK]		= false;
			theBehaviors[AVOIDDEADEND].Initialize		(AVOIDDEADEND, 15, 1, 1, theInhibits, ptrs);
			theInhibits[AVOID_IR]			= false;
			theInhibits[AVOIDVISUAL]		= false;
			theInhibits[AVOIDTRAP]			= false;
			theInhibits[AVOIDDEADEND]		= false;
			theInhibits[FORWARD]			= false;
			theInhibits[GLANCERIGHT]		= false;
			theInhibits[GLANCELEFT]			= false;
			theInhibits[SCANSEARCH]			= false;
			theInhibits[LANDMARKORIENT]		= false;
			theInhibits[FEEDERORIENT]		= false;
			theInhibits[SCANASSOCIATE]		= false;
			theInhibits[POSTFEEDSCANASSOCIATE]		= false;
			theInhibits[ESCAPECORNER]		= false;
			theInhibits[ESCAPESTUCK]		= false;
			theBehaviors[FORWARD].Initialize			(FORWARD, 0, 0, 0, theInhibits, ptrs);
			theInhibits[AVOID_IR]			= true;
			theInhibits[AVOIDVISUAL]		= false;
			theInhibits[AVOIDTRAP]			= false;
			theInhibits[AVOIDDEADEND]		= true;
			theInhibits[FORWARD]			= true;
			theInhibits[GLANCERIGHT]		= false;
			theInhibits[GLANCELEFT]			= false;
			theInhibits[SCANSEARCH]			= false;
			theInhibits[LANDMARKORIENT]		= false;
			theInhibits[FEEDERORIENT]		= false;
			theInhibits[SCANASSOCIATE]		= false;
			theInhibits[POSTFEEDSCANASSOCIATE]		= false;
			theInhibits[ESCAPECORNER]		= false;
			theInhibits[ESCAPESTUCK]		= false;
			theBehaviors[GLANCERIGHT].Initialize		(GLANCERIGHT, 13, 1, 1, theInhibits, ptrs);
			theInhibits[AVOID_IR]			= true;
			theInhibits[AVOIDVISUAL]		= false;
			theInhibits[AVOIDTRAP]			= false;
			theInhibits[AVOIDDEADEND]		= true;
			theInhibits[FORWARD]			= true;
			theInhibits[GLANCERIGHT]		= false;
			theInhibits[GLANCELEFT]			= false;
			theInhibits[SCANSEARCH]			= false;
			theInhibits[LANDMARKORIENT]		= false;
			theInhibits[FEEDERORIENT]		= false;
			theInhibits[SCANASSOCIATE]		= false;
			theInhibits[POSTFEEDSCANASSOCIATE]		= false;
			theInhibits[ESCAPECORNER]		= false;
			theInhibits[ESCAPESTUCK]		= false;
			theBehaviors[GLANCELEFT].Initialize			(GLANCELEFT, 13, 1, 1, theInhibits, ptrs);
			theInhibits[AVOID_IR]			= true;
			theInhibits[AVOIDVISUAL]		= true;
			theInhibits[AVOIDTRAP]			= true;
			theInhibits[AVOIDDEADEND]		= true;
			theInhibits[FORWARD]			= true;
			theInhibits[GLANCERIGHT]		= true;
			theInhibits[GLANCELEFT]			= true;
			theInhibits[SCANSEARCH]			= false;
			theInhibits[LANDMARKORIENT]		= false;
			theInhibits[FEEDERORIENT]		= false;
			theInhibits[SCANASSOCIATE]		= false;
			theInhibits[POSTFEEDSCANASSOCIATE]		= false;
			theInhibits[ESCAPECORNER]		= false;
			theInhibits[ESCAPESTUCK]		= false;
			theBehaviors[SCANSEARCH].Initialize			(SCANSEARCH, 25, 1, 1, theInhibits, ptrs);
			theInhibits[AVOID_IR]			= false;
			theInhibits[AVOIDVISUAL]		= false;
			theInhibits[AVOIDTRAP]			= false;
			theInhibits[AVOIDDEADEND]		= false;
			theInhibits[FORWARD]			= true;
			theInhibits[GLANCERIGHT]		= false;
			theInhibits[GLANCELEFT]			= false;
			theInhibits[SCANSEARCH]			= true;
			theInhibits[LANDMARKORIENT]		= false;
			theInhibits[FEEDERORIENT]		= false;
			theInhibits[SCANASSOCIATE]		= false;
			theInhibits[POSTFEEDSCANASSOCIATE]		= false;
			theInhibits[ESCAPECORNER]		= false;
			theInhibits[ESCAPESTUCK]		= false;
			theBehaviors[LANDMARKORIENT].Initialize		(LANDMARKORIENT, 30, 1, 1, theInhibits, ptrs);
			theInhibits[AVOID_IR]			= false;
			theInhibits[AVOIDVISUAL]		= false;
			theInhibits[AVOIDTRAP]			= false;
			theInhibits[AVOIDDEADEND]		= true;
			theInhibits[FORWARD]			= true;
			theInhibits[GLANCERIGHT]		= true;
			theInhibits[GLANCELEFT]			= true;
			theInhibits[SCANSEARCH]			= true;
			theInhibits[LANDMARKORIENT]		= true;
			theInhibits[FEEDERORIENT]		= false;
			theInhibits[SCANASSOCIATE]		= false;
			theInhibits[POSTFEEDSCANASSOCIATE]		= false;
			theInhibits[ESCAPECORNER]		= false;
			theInhibits[ESCAPESTUCK]		= false;
			theBehaviors[FEEDERORIENT].Initialize		(FEEDERORIENT, 20, 1, 1, theInhibits, ptrs);
			theInhibits[AVOID_IR]			= false;
			theInhibits[AVOIDVISUAL]		= false;
			theInhibits[AVOIDTRAP]			= false;
			theInhibits[AVOIDDEADEND]		= false;
			theInhibits[FORWARD]			= false;
			theInhibits[GLANCERIGHT]		= false;
			theInhibits[GLANCELEFT]			= false;
			theInhibits[SCANSEARCH]			= false;
			theInhibits[LANDMARKORIENT]		= false;
			theInhibits[FEEDERORIENT]		= false;
			theInhibits[SCANASSOCIATE]		= false;
			theInhibits[POSTFEEDSCANASSOCIATE]		= false;
			theInhibits[ESCAPECORNER]		= false;
			theInhibits[ESCAPESTUCK]		= false;
			theBehaviors[SCANASSOCIATE].Initialize		(SCANASSOCIATE, 75, 1, 1, theInhibits, ptrs);
			theInhibits[AVOID_IR]			= false;
			theInhibits[AVOIDVISUAL]		= false;
			theInhibits[AVOIDTRAP]			= false;
			theInhibits[AVOIDDEADEND]		= false;
			theInhibits[FORWARD]			= false;
			theInhibits[GLANCERIGHT]		= false;
			theInhibits[GLANCELEFT]			= false;
			theInhibits[SCANSEARCH]			= false;
			theInhibits[LANDMARKORIENT]		= false;
			theInhibits[FEEDERORIENT]		= false;
			theInhibits[SCANASSOCIATE]		= false;
			theInhibits[POSTFEEDSCANASSOCIATE]		= false;
			theInhibits[ESCAPECORNER]		= false;
			theInhibits[ESCAPESTUCK]		= false;
			theBehaviors[POSTFEEDSCANASSOCIATE].Initialize		(POSTFEEDSCANASSOCIATE, 75, 1, 1, theInhibits, ptrs);
			theInhibits[AVOID_IR]			= true;
			theInhibits[AVOIDVISUAL]		= true;
			theInhibits[AVOIDTRAP]			= true;
			theInhibits[AVOIDDEADEND]		= true;
			theInhibits[FORWARD]			= true;
			theInhibits[GLANCERIGHT]		= true;
			theInhibits[GLANCELEFT]			= true;
			theInhibits[SCANSEARCH]			= true;
			theInhibits[LANDMARKORIENT]		= true;
			theInhibits[FEEDERORIENT]		= true;
			theInhibits[SCANASSOCIATE]		= true;
			theInhibits[POSTFEEDSCANASSOCIATE]		= true;
			theInhibits[ESCAPECORNER]		= false;
			theInhibits[ESCAPESTUCK]		= false;
			theBehaviors[ESCAPECORNER].Initialize		(ESCAPECORNER, 60, 1, 1, theInhibits, ptrs);
			theInhibits[AVOID_IR]			= false;
			theInhibits[AVOIDVISUAL]		= false;
			theInhibits[AVOIDTRAP]			= false;
			theInhibits[AVOIDDEADEND]		= false;
			theInhibits[FORWARD]			= false;
			theInhibits[GLANCERIGHT]		= false;
			theInhibits[GLANCELEFT]			= false;
			theInhibits[SCANSEARCH]			= false;
			theInhibits[LANDMARKORIENT]		= false;
			theInhibits[FEEDERORIENT]		= false;
			theInhibits[SCANASSOCIATE]		= false;
			theInhibits[POSTFEEDSCANASSOCIATE]		= false;
			theInhibits[ESCAPECORNER]		= false;
			theInhibits[ESCAPESTUCK]		= false;
			theBehaviors[ESCAPESTUCK].Initialize		(ESCAPESTUCK, 15, 1, 1, theInhibits, ptrs);
			break;
	}
}

int16 Mood::RunMood(int16 &theLeft, int16 &theRight)
{
	left = right = 0;
	int16 returnValue = NOBEH;
	
	for (int8 i = 0; i < NUMBEHAVIORS; i++)
		theBehaviors[i].inhibited = false;
	
	theBehaviors[ESCAPESTUCK].RunBehavior(left, right);
	theBehaviors[ESCAPECORNER].RunBehavior(left, right);
	if (theBehaviors[POSTFEEDSCANASSOCIATE].RunBehavior(left, right))
		returnValue = POSTFEEDSCANASSOCIATE;
	if (theBehaviors[SCANASSOCIATE].RunBehavior(left, right))
		returnValue = SCANASSOCIATE;
	if (theBehaviors[FEEDERORIENT].RunBehavior(left, right))
		returnValue = FEEDERORIENT;
	if (theBehaviors[LANDMARKORIENT].RunBehavior(left, right))
		returnValue = LANDMARKORIENT;
	theBehaviors[SCANSEARCH].RunBehavior(left, right);
	theBehaviors[GLANCELEFT].RunBehavior(left, right);
	theBehaviors[GLANCERIGHT].RunBehavior(left, right);
	theBehaviors[FORWARD].RunBehavior(left, right);
	theBehaviors[AVOIDDEADEND].RunBehavior(left, right);
	theBehaviors[AVOIDTRAP].RunBehavior(left, right);
	theBehaviors[AVOIDVISUAL].RunBehavior(left, right);
	theBehaviors[AVOID_IR].RunBehavior(left, right);
	
	theLeft = left;
	theRight = right;
	
	return returnValue;
}
//  MOOD  *******************************************************
//  THE KHEPERA  *******************************************************
class TheKhepera
{
	private:
		Mood theMoods[3];
						
		//Memorized landmark stuff
		int16 numMemorizedLandmarks;			//present number of memorized landmarks
		LandmarkMemory landmarkMemories[MAXNUMMEMORIZEDLANDMARKS];
		
		//Step Setup
		void GetStepData();
		
		//Step Run
		int16 RunAMood();
		
		//Actions
		void SetGoalAndthePresentLandmark();
		
	public:
		int16 left, right;	//motor commands
		
		TheKhepera();
		~TheKhepera();
		void Copy(TheKhepera &tk);
		int16 GetLandmarkMemoryMatch(int16 id);
		int16 CheckAndAddLandmarkMemory(int16 id);
		bool CheckAndAddLandmarkAssociation(int16 id);
		bool CheckAndAddFeederAssociation(int16 id);
		void CalculateOneStep();
};

TheKhepera::TheKhepera()
{
	theMoods[SEARCHING].Initialize(SEARCHING);
	
	numMemorizedLandmarks = 0;
	thePresentTimeStepData.presentLandmarkMemoryIndex = -1;
	
	left = right = 0;
}

TheKhepera::~TheKhepera()
{
}

void TheKhepera::Copy(TheKhepera &tk)
{
	for (int8 i = 0; i < 3; i++)
		theMoods[i] = tk.theMoods[i];
		
	left = tk.left;
	right = tk.right;
}

void TheKhepera::GetStepData()
{
	for (int8 i = 0; i < 8; i++)
		IRsensors[i] = khepera_get_proximity(i);
	
	rawImage = k6300_get_image();
	
	CreateFilteredImages();
}

int16 TheKhepera::RunAMood()
{
	return theMoods[SEARCHING].RunMood(left, right);
}

//set the desired goal landmark type and determine is the robot is presently located at a landmark site.
void TheKhepera::SetGoalAndthePresentLandmark()
{
	int16 lowestLandmarkBound = 0;
	int16 highestCornerLandmarkBound = k6300_heightMinus1;
	int16 presentIndex = -1;
	int16 goalIndex = -1;
	int16 cornerGoalIndex = -1;
	bool goalSoFarNeedsAssociating = false;
	bool goalSoFarIsReallyOld = false;
	static int16 lastGoal = -1;
	
	thePresentTimeStepData.presentLandmarkOutOfView = true;
			
	for (int8 i = 0; i < theImmediateViewKnowledge.numImmediateLandmarks; i++)
	{
		if (theImmediateViewKnowledge.visibleLandmarks[i] < RED_CORNER)
		{
			//check to see if the indexed landmark should be made the new goal
			if (goalSoFarNeedsAssociating)
			{
				if (theWorldKnowledge.needsAssociating[theImmediateViewKnowledge.visibleLandmarks[i] - 8] &&
					theImmediateViewKnowledge.landmarkBoxes[i].bottom > lowestLandmarkBound)
					{
						goalSoFarNeedsAssociating = true;	//this line actually shouldn't be necessary, but this makes sure the variable gets set.
						lowestLandmarkBound = theImmediateViewKnowledge.landmarkBoxes[i].bottom;
						goalIndex = i;
					}
			}
			else	//the goal chosen so far does not need associating
			{
				if (theWorldKnowledge.needsAssociating[theImmediateViewKnowledge.visibleLandmarks[i] - 8])
				{
					goalSoFarNeedsAssociating = true;
					lowestLandmarkBound = theImmediateViewKnowledge.landmarkBoxes[i].bottom;
					goalIndex = i;
				}
				if (!goalSoFarIsReallyOld)
				{
					if (theWorldKnowledge.timeSinceLastGoal[theImmediateViewKnowledge.visibleLandmarks[i] - 8] > oldLandmarkGoalAge &&
						!theWorldKnowledge.feederVisible[theImmediateViewKnowledge.visibleLandmarks[i] - 8] != FEEDER_NOT_VISIBLE)
					{
						goalSoFarIsReallyOld = true;
						lowestLandmarkBound = theImmediateViewKnowledge.landmarkBoxes[i].bottom;
						goalIndex = i;
					}
				}
				else if (theWorldKnowledge.timeSinceLastGoal[theImmediateViewKnowledge.visibleLandmarks[i] - 8] >
						theWorldKnowledge.timeSinceLastGoal[theImmediateViewKnowledge.visibleLandmarks[goalIndex] - 8] &&
					!theWorldKnowledge.feederVisible[theImmediateViewKnowledge.visibleLandmarks[i] - 8] != FEEDER_NOT_VISIBLE)
					{
						goalSoFarIsReallyOld = true;	//this line actually shouldn't be necessary, but this makes sure the variable gets set.
						lowestLandmarkBound = theImmediateViewKnowledge.landmarkBoxes[i].bottom;
						goalIndex = i;
					}
			}
			
			//check to see if the indexed landmark is the present landmark (located right where the robot is on the map)
			if (theImmediateViewKnowledge.landmarkBoxes[i].bottom == k6300_heightMinus1 && theImmediateViewKnowledge.landmarkBoxes[i].left < k6300_halfWidth &&
				theImmediateViewKnowledge.landmarkBoxes[i].right >= k6300_halfWidth && IRsensors[2] + IRsensors[3] > 300)
			{
				presentIndex = lastGoal = i;
				thePresentTimeStepData.presentLandmarkOutOfView = false;
			}
		}
		else
		{
			if (theImmediateViewKnowledge.landmarkBoxes[i].top < highestCornerLandmarkBound && lastGoal != theImmediateViewKnowledge.visibleLandmarks[i])
			{
				highestCornerLandmarkBound = theImmediateViewKnowledge.landmarkBoxes[i].top;
				cornerGoalIndex = i;
			}
			
			if (theImmediateViewKnowledge.landmarkBoxes[i].top < 6 && IRsensors[1] + IRsensors[4] > 300)
			{
				presentIndex = lastGoal = i;
				thePresentTimeStepData.presentLandmarkOutOfView = false;
			}
		}
	}
	
	//choose between closest landmark and corner
	if (goalIndex == -1)
	{
		if (cornerGoalIndex != -1)
			goalIndex = cornerGoalIndex;
	}
	else if (cornerGoalIndex != -1)
	{
		int16 distanceFromBottom = 60 - lowestLandmarkBound;
		
		int16 scaledDistanceFromBottom = 30 - distanceFromBottom;
		
		if (distanceFromBottom + 2 - scaledDistanceFromBottom > highestCornerLandmarkBound)
			goalIndex = cornerGoalIndex;
	}
	
	if (goalIndex != -1)
	{
		thePresentTimeStepData.presentLandmarkGoal = theImmediateViewKnowledge.visibleLandmarks[goalIndex];
		thePresentTimeStepData.presentLandmarkGoalMemoryIndex = goalIndex;
		thePresentTimeStepData.presentLandmarkGoalBox = theImmediateViewKnowledge.landmarkBoxes[goalIndex];
		if (theImmediateViewKnowledge.landmarkDistances[goalIndex] <= 4)
			theWorldKnowledge.timeSinceLastGoal[thePresentTimeStepData.presentLandmarkGoal - 8] = 0;
	}
	else
	{
		thePresentTimeStepData.presentLandmarkGoal = BLANK;
		thePresentTimeStepData.presentLandmarkGoalBox = zeroDimBox;
	}
	
	if (!theMoods[SEARCHING].theBehaviors[SCANASSOCIATE].active)
	{
		if (presentIndex != -1)
		{
			thePresentTimeStepData.presentLandmark = theImmediateViewKnowledge.visibleLandmarks[presentIndex];
			thePresentTimeStepData.presentLandmarkMemoryIndex = CheckAndAddLandmarkMemory(theImmediateViewKnowledge.visibleLandmarks[presentIndex]);
		}
		else
		{
			thePresentTimeStepData.presentLandmark = -1;
			thePresentTimeStepData.presentLandmarkMemoryIndex = -1;
		}
	}
}

//returns the index of the landmark memory that the landmark matches
//returns -1 if no landmark memory matches the landmark
int16 TheKhepera::GetLandmarkMemoryMatch(int16 id)
{
	for (int8 i = 0; i < numMemorizedLandmarks; i++)
		if (landmarkMemories[i].id == id)
			return i;
	
	return -1;
}

//returns the memory index (new or old) if there was enough room to add a new landmark memory or an old memory matched the id
//return -1 if there wasn't enough room
int16 TheKhepera::CheckAndAddLandmarkMemory(int16 id)
{
	int16 memoryIndex;
	
	if (numMemorizedLandmarks < MAXNUMMEMORIZEDLANDMARKS)
	{
		memoryIndex = GetLandmarkMemoryMatch(id);
		
		if (memoryIndex != -1)
			return memoryIndex;
		else
		{
			landmarkMemories[numMemorizedLandmarks].id = id;
			
			numMemorizedLandmarks++;
		}
		
		return numMemorizedLandmarks - 1;
	}
	
	return -1;
}

//returns true if the sited landmark hasn't been associated yet.  This creates an association.
//returns false if the sited landmark is already associated with the present landmark
bool TheKhepera::CheckAndAddLandmarkAssociation(int16 id)
{
	if (landmarkMemories[thePresentTimeStepData.presentLandmarkMemoryIndex].id != id && !landmarkMemories[thePresentTimeStepData.presentLandmarkMemoryIndex].IsLandmarkAssociated(id))
	{
		landmarkMemories[thePresentTimeStepData.presentLandmarkMemoryIndex].AddAssociation(id);
		
		return true;
	}
	
	return false;
}

//returns true if the sited landmark hasn't been feeder associated yet.  This creates an association.
//returns false if the sited landmark is already associated with a feeder or if there isn't enough room to add the landmark memory
bool TheKhepera::CheckAndAddFeederAssociation(int16 id)
{
	int16 index = -1;
	
	for (int8 i = 0; i < numMemorizedLandmarks; i++)
		if (landmarkMemories[i].id == id)
		{
			index = i;
			i = numMemorizedLandmarks;
		}
	
	if (index == -1)
	{
		index = CheckAndAddLandmarkMemory(id);
		
		if (index != -1)
			landmarkMemories[index].feederAssociation = true;
		else return false;
		
		return true;
	}
	else landmarkMemories[index].feederAssociation = true;
		
	return false;
}

//Update the visual and IR data, pick a goal, determine any present landmark, run the brain,
//make landmark associations if necessary, and update the khepera motor values
void TheKhepera::CalculateOneStep()
{
	bool preferableFeeder = false;							//is the feeder preferable over others found during the scan?
	static int16 maxFeederHeight = -1;						//closest feeder yet found during scan.
	static int16 maxCornerWidth = -1;						//closest corner yet found during scan.
	static int16 maxLandmarkHeight = -1;					//closest landmark yet found during scan.
	static int16 lowestSkyBottom = -1;						//furthest sky yet found durin scan.
	static int16 lowestLandmarkBottom = -1;					//closest landmark yet found during scan.
	static int16 highestCornerTop = K6300_HEIGHT;			//closest corner yet found during scan.
	static bool closestLandmarkNeedsAssociating = false;	//does the presently desired landmark need associating?
	static int16 interestingObjectCounter = -1;				//counter used to determine which direction to turn after scan to get to desired object.
	static int16 timeElapsed = 0;							//time elapsed during scan.
	
	GetStepData();
	
	SetGoalAndthePresentLandmark();
	
	left = right = 0;	//only necessary for debugging (hopefully).  Better just to leave it in though.
	
	if (recentlyFeederOrienting > 0)
		recentlyFeederOrienting--;
	if (recentlyFed > 0)
		recentlyFed--;
	
	//avoidVisualHeight = 16;
	leftAvoidVisualHeight = rightAvoidVisualHeight = 16;
	
	int16 action = RunAMood();
	
	for (int i = 0; i < 7; i++)
		theWorldKnowledge.timeSinceLastGoal[i]++;
	
	initializeAllBehaviors = false;
	
	if (action == SCANASSOCIATE)
	{
		UpdateLandmarkDistanceRelationships(thePresentTimeStepData.presentLandmark);	//update distances from present landmark to visible landmarks.
		
		scanning = true;
		
		for (int8 i = 0; i < theImmediateViewKnowledge.numImmediateLandmarks; i++)
			CheckAndAddLandmarkAssociation(theImmediateViewKnowledge.visibleLandmarks[i]);	//add associations between the present landmark and visible landmarks.
		
		if (feederOnDetected || feederOffDetected)
		{
			CheckAndAddFeederAssociation(thePresentTimeStepData.presentLandmark);
			theWorldKnowledge.feederVisible[thePresentTimeStepData.presentLandmark - 8] = FEEDER_IS_VISIBLE;
		}
		
		if (!seekingFeeder)
		{
			if (feederOnBound.bottom == k6300_heightMinus1 || feederOnBound.height > maxFeederHeight)
				preferableFeeder = true;
			else if (feederOnDetected)
				preferableFeeder = true;
		}
		else
		{
			if (feederOnDetected)
				if (feederOnBound.bottom == k6300_heightMinus1 || feederOnBound.height > maxFeederHeight)
					preferableFeeder = true;
		}
		
		if (preferableFeeder)
		{
			maxFeederHeight = feederOnBound.height;
			
			desiredFeederHeight = maxFeederHeight - 3;
			if (desiredFeederHeight < 1)
				desiredFeederHeight = 1;
				
			if (feederOnDetected)
				seekingFeeder = true;
			
			interestingObjectCounter = 0;
		}
		
		if (maxFeederHeight == -1)		//Only look for landmarks if there haven't been any feeders.
		{
			for (int8 i = 0; i < theImmediateViewKnowledge.numImmediateLandmarks; i++)
			{
				if (theImmediateViewKnowledge.visibleLandmarks[i] < RED_CORNER)	//boxes and the disk
				{
					if (theImmediateViewKnowledge.visibleLandmarks[i] != thePresentTimeStepData.presentLandmark)
						if (!closestLandmarkNeedsAssociating ||
							(theWorldKnowledge.needsAssociating[theImmediateViewKnowledge.visibleLandmarks[i] - 8] && theImmediateViewKnowledge.landmarkBoxes[i].height > maxLandmarkHeight))
						{
							int16 distanceFromBottom = 60 - theImmediateViewKnowledge.landmarkBoxes[i].bottom;
							
							int16 scaledDistanceFromBottom = 30 - distanceFromBottom;
							
							if (distanceFromBottom + 2 - scaledDistanceFromBottom < highestCornerTop || theWorldKnowledge.needsAssociating[theImmediateViewKnowledge.visibleLandmarks[i] - 8])
							{
								closestLandmarkNeedsAssociating = theWorldKnowledge.needsAssociating[theImmediateViewKnowledge.visibleLandmarks[i] - 8];
								maxLandmarkHeight = theImmediateViewKnowledge.landmarkBoxes[i].height;
								lowestLandmarkBottom = theImmediateViewKnowledge.landmarkBoxes[i].bottom;
								interestingObjectCounter = 0;
							}
						}
				}
				else	//corners
				{
					if (theImmediateViewKnowledge.visibleLandmarks[i] != thePresentTimeStepData.presentLandmark)
						if (!closestLandmarkNeedsAssociating ||
							(theWorldKnowledge.needsAssociating[theImmediateViewKnowledge.visibleLandmarks[i] - 8] && theImmediateViewKnowledge.landmarkBoxes[i].width > maxCornerWidth))
						{
							int16 distanceFromBottom = 60 - lowestLandmarkBottom;
							
							int16 scaledDistanceFromBottom = 30 - distanceFromBottom;
							
							if (distanceFromBottom + 2 - scaledDistanceFromBottom > highestCornerTop || theWorldKnowledge.needsAssociating[theImmediateViewKnowledge.visibleLandmarks[i] - 8])
							{
								closestLandmarkNeedsAssociating = theWorldKnowledge.needsAssociating[theImmediateViewKnowledge.visibleLandmarks[i] - 8];
								maxCornerWidth = theImmediateViewKnowledge.landmarkBoxes[i].width;
								highestCornerTop = theImmediateViewKnowledge.landmarkBoxes[i].top;
								interestingObjectCounter = 0;
							}
						}
				}
			}
		}
		else maxLandmarkHeight = -1;	//There is a feeder visible so ignore landmarks.
		
		if (maxFeederHeight == -1 && maxLandmarkHeight == -1)	//Only look for sky if there haven't been any feeders or landmarks.
		{
			if (skyBottom > lowestSkyBottom)
			{
				lowestSkyBottom = skyBottom;
				interestingObjectCounter = 0;
			}
		}
		else lowestSkyBottom = -1;	//There is a feeder or landmark so ignore sky.
		
		if (theWorldKnowledge.feederVisible[thePresentTimeStepData.presentLandmark - 8] != FEEDER_IS_VISIBLE)
			theWorldKnowledge.feederVisible[thePresentTimeStepData.presentLandmark - 8] = FEEDER_NOT_VISIBLE;
		
		timeElapsed++;
		if (interestingObjectCounter != -1)
		{
			if (interestingObjectCounter > timeElapsed / 2)
				postScanTurnAmount = interestingObjectCounter - timeElapsed - 2;
			else postScanTurnAmount = interestingObjectCounter + 2;
			interestingObjectCounter++;
		}
	}
	else if (action == POSTFEEDSCANASSOCIATE)
	{
		UpdateLandmarkDistanceRelationships(FEEDER_ON);	//update distances from present feeder to visible landmarks.
		
		scanning = true;
		
		for (int8 i = 0; i < theImmediateViewKnowledge.numImmediateLandmarks; i++)
		{
			CheckAndAddFeederAssociation(theImmediateViewKnowledge.visibleLandmarks[i]);	//add feeder associations to visible landmarks.
			theWorldKnowledge.feederVisible[theImmediateViewKnowledge.visibleLandmarks[i] - 8] = FEEDER_IS_VISIBLE;
		}
		
		if (!seekingFeeder)
		{
			if (feederOnBound.bottom == k6300_heightMinus1 || feederOnBound.height > maxFeederHeight)
				preferableFeeder = true;
			else if (feederOnDetected)
				preferableFeeder = true;
		}
		else
		{
			if (feederOnDetected)
				if (feederOnBound.bottom == k6300_heightMinus1 || feederOnBound.height > maxFeederHeight)
					preferableFeeder = true;
		}
		
		if (preferableFeeder)
		{
			maxFeederHeight = feederOnBound.height;
			
			desiredFeederHeight = maxFeederHeight - 3;
			if (desiredFeederHeight < 1)
				desiredFeederHeight = 1;
			
			if (feederOnDetected)
				seekingFeeder = true;
			
			interestingObjectCounter = 0;
		}
		
		if (maxFeederHeight == -1)		//Only look for landmarks if there haven't been any feeders.
		{
			for (int8 i = 0; i < theImmediateViewKnowledge.numImmediateLandmarks; i++)
			{
				if (theImmediateViewKnowledge.visibleLandmarks[i] < RED_CORNER)	//boxes and the disk
				{
					if (theImmediateViewKnowledge.visibleLandmarks[i] != thePresentTimeStepData.presentLandmark)
						if (!closestLandmarkNeedsAssociating ||
							(theWorldKnowledge.needsAssociating[theImmediateViewKnowledge.visibleLandmarks[i] - 8] && theImmediateViewKnowledge.landmarkBoxes[i].height > maxLandmarkHeight))
						{
							int16 distanceFromBottom = 60 - theImmediateViewKnowledge.landmarkBoxes[i].bottom;
							
							int16 scaledDistanceFromBottom = 30 - distanceFromBottom;
							
							if (distanceFromBottom + 2 - scaledDistanceFromBottom < highestCornerTop || theWorldKnowledge.needsAssociating[theImmediateViewKnowledge.visibleLandmarks[i] - 8])
							{
								closestLandmarkNeedsAssociating = theWorldKnowledge.needsAssociating[theImmediateViewKnowledge.visibleLandmarks[i] - 8];
								maxLandmarkHeight = theImmediateViewKnowledge.landmarkBoxes[i].height;
								lowestLandmarkBottom = theImmediateViewKnowledge.landmarkBoxes[i].bottom;
								interestingObjectCounter = 0;
							}
						}
				}
				else	//corners
				{
					if (theImmediateViewKnowledge.visibleLandmarks[i] != thePresentTimeStepData.presentLandmark)
						if (!closestLandmarkNeedsAssociating ||
							(theWorldKnowledge.needsAssociating[theImmediateViewKnowledge.visibleLandmarks[i] - 8] && theImmediateViewKnowledge.landmarkBoxes[i].width > maxCornerWidth))
						{
							int16 distanceFromBottom = 60 - lowestLandmarkBottom;
							
							int16 scaledDistanceFromBottom = 30 - distanceFromBottom;
							
							if (distanceFromBottom + 2 - scaledDistanceFromBottom > highestCornerTop || theWorldKnowledge.needsAssociating[theImmediateViewKnowledge.visibleLandmarks[i] - 8])
							{
								closestLandmarkNeedsAssociating = theWorldKnowledge.needsAssociating[theImmediateViewKnowledge.visibleLandmarks[i] - 8];
								maxCornerWidth = theImmediateViewKnowledge.landmarkBoxes[i].width;
								highestCornerTop = theImmediateViewKnowledge.landmarkBoxes[i].top;
								interestingObjectCounter = 0;
							}
						}
				}
			}
		}
		else maxLandmarkHeight = -1;	//There is a feeder visible so ignore landmarks.
		
		if (maxFeederHeight == -1 && maxLandmarkHeight == -1)	//Only look for sky if there haven't been any feeders or landmarks.
		{
			if (skyBottom > lowestSkyBottom)
			{
				lowestSkyBottom = skyBottom;
				interestingObjectCounter = 0;
			}
		}
		else lowestSkyBottom = -1;	//There is a feeder or landmark so ignore sky.
		
		timeElapsed++;
		if (interestingObjectCounter != -1)
		{
			if (interestingObjectCounter > timeElapsed / 2)
				postScanTurnAmount = interestingObjectCounter - timeElapsed - 2;
			else postScanTurnAmount = interestingObjectCounter + 2;
			interestingObjectCounter++;
		}
	}
	else
	{
		maxFeederHeight = -1;
		maxCornerWidth = -1;
		maxLandmarkHeight = -1;
		lowestSkyBottom = -1;
		lowestLandmarkBottom = -1;
		highestCornerTop = K6300_HEIGHT;
		closestLandmarkNeedsAssociating = false;
		interestingObjectCounter = -1;
		timeElapsed = 0;
		postScanTurnAmount = 0;
		scanning = false;
	}
		
	khepera_set_speed(LEFT, left);
	khepera_set_speed(RIGHT, right);
}
//  THE KHEPERA  *******************************************************

//returns a random number 0 <= X < range
int16 Random(int16 range)
{
	return (rand() % range);
}

//Creates some initial global arrays and boxes
void InitializeData()
{
	zeroDimBox.left = zeroDimBox.right = zeroDimBox.top = zeroDimBox.bottom = zeroDimBox.width = zeroDimBox.height = -1;
	
	insideOutBox.left = k6300_widthMinus1;
	insideOutBox.top = k6300_heightMinus1;
	insideOutBox.right = 0;
	insideOutBox.bottom = 0;
	insideOutBox.width = -K6300_WIDTH;
	insideOutBox.height = -K6300_HEIGHT;
	
	for (int8 i = 0; i < 7; i++)
	{
		theWorldKnowledge.feederDistances[i] = 99;
		for (int8 j = 0; j < 7; j++)
		{
			theWorldKnowledge.landmarkSeparationDistances[i][j] = 99;
			for (int8 k = 0; k < 7; k++)
				theWorldKnowledge.landmarkSeparationAngles[i][j][k] = 0;
		}
		theWorldKnowledge.timeSinceLastGoal[i] = 0;
		theWorldKnowledge.needsAssociating[i] = true;
		theWorldKnowledge.feederVisible[i] = FEEDER_VISIBLE_UNKNOWN;
	}
	
	theImmediateViewKnowledge.numImmediateLandmarks = 0;
	for (int8 i = 0; i < MAXNUMMEMORIZEDLANDMARKS; i++)
	{
		theImmediateViewKnowledge.visibleLandmarks[i] = BLANK;
		theImmediateViewKnowledge.landmarkBoxes[i] = zeroDimBox;
		theImmediateViewKnowledge.landmarkDistances[i] = 99;
	}
	
	thePresentTimeStepData.presentLandmark = BLANK;
	thePresentTimeStepData.presentLandmarkOutOfView = true;
	thePresentTimeStepData.presentLandmarkMemoryIndex = 0;
	thePresentTimeStepData.presentLandmarkGoal = BLANK;
	thePresentTimeStepData.presentLandmarkGoalMemoryIndex = 0;
	thePresentTimeStepData.presentLandmarkGoalBox = zeroDimBox;
	
	//set up distance lookup tables for the different kinds of objects.
	//box periferal
	boxPeriferal[0] = 15;	boxPeriferal[1] = 15;	boxPeriferal[2] = 15;	boxPeriferal[3] = 15;	boxPeriferal[4] = 15;	boxPeriferal[5] = 15;	boxPeriferal[6] =  14;	boxPeriferal[7] = 12;	boxPeriferal[8] = 10;	boxPeriferal[9] = 8;
	boxPeriferal[10] = 7;	boxPeriferal[11] = 6;	boxPeriferal[12] = 6;	boxPeriferal[13] = 6;	boxPeriferal[14] = 5;	boxPeriferal[15] = 5;	boxPeriferal[16] = 5;	boxPeriferal[17] = 4;	boxPeriferal[18] = 4;	boxPeriferal[19] = 4;
	boxPeriferal[20] = 4;	boxPeriferal[21] = 4;	boxPeriferal[22] = 3;	boxPeriferal[23] = 3;	boxPeriferal[24] = 3;	boxPeriferal[25] = 3;
	
	//box centered
	boxCentered[0] = 15;	boxCentered[1] = 15;	boxCentered[2] = 15;	boxCentered[3] = 15;	boxCentered[4] = 15;	boxCentered[5] = 12;	boxCentered[6] = 10;	boxCentered[7] = 8;	boxCentered[8] = 7;	boxCentered[9] = 6;
	boxCentered[10] = 6;	boxCentered[11] = 5;	boxCentered[12] = 5;	boxCentered[13] = 4;	boxCentered[14] = 4;	boxCentered[15] = 4;	boxCentered[16] = 4;	boxCentered[17] = 4;	boxCentered[18] = 3;	boxCentered[19] = 3;
	boxCentered[20] = 3;	boxCentered[21] = 3;	boxCentered[22] = 3;	boxCentered[23] = 3;	boxCentered[24] = 3;	boxCentered[25] = 3;
	
	//disk periferal
	diskPeriferal[0] = 15;	diskPeriferal[1] = 15;	diskPeriferal[2] = 15;	diskPeriferal[3] = 15;	diskPeriferal[4] = 10;	diskPeriferal[5] = 9;	diskPeriferal[6] = 8;	diskPeriferal[7] = 7;	diskPeriferal[8] = 6;	diskPeriferal[9] = 5;
	diskPeriferal[10] = 5;	diskPeriferal[11] = 4;	diskPeriferal[12] = 4;	diskPeriferal[13] = 4;	diskPeriferal[14] = 4;	diskPeriferal[15] = 3;	diskPeriferal[16] = 3;	diskPeriferal[17] = 3;
	
	//disk centered
	diskCentered[0] = 15;	diskCentered[1] = 15;	diskCentered[2] = 15;	diskCentered[3] = 15;	diskCentered[4] = 10;	diskCentered[5] = 8;	diskCentered[6] = 7;	diskCentered[7] = 6;	diskCentered[8] = 5;	diskCentered[9] = 5;
	diskCentered[10] = 4;	diskCentered[11] = 4;	diskCentered[12] = 4;	diskCentered[13] = 3;	diskCentered[14] = 3;	diskCentered[15] = 3;	diskCentered[16] = 3;	diskCentered[17] = 3;
	
	//corner periferal
	cornerPeriferal[0] = 15;	cornerPeriferal[1] = 15;	cornerPeriferal[2] = 15;	cornerPeriferal[3] = 15;	cornerPeriferal[4] = 10;	cornerPeriferal[5] = 8;	cornerPeriferal[6] = 7;	cornerPeriferal[7] = 6;	cornerPeriferal[8] = 5;	cornerPeriferal[9] = 4;
	cornerPeriferal[10] = 4;	cornerPeriferal[11] = 3;	cornerPeriferal[12] = 3;
	
	//corner centered
	cornerCentered[0] = 15;	cornerCentered[1] = 15;	cornerCentered[2] = 15;	cornerCentered[3] = 8;	cornerCentered[4] = 6;	cornerCentered[5] = 5;	cornerCentered[6] = 4;	cornerCentered[7] = 4;	cornerCentered[8] = 3;	cornerCentered[9] = 3;
	cornerCentered[10] = 3;	cornerCentered[11] = 3;	cornerCentered[12] = 3;
	
	//feeder periferal
	feederPeriferal[0] = 15;	feederPeriferal[1] = 15;	feederPeriferal[2] = 15;	feederPeriferal[3] = 15;	feederPeriferal[4] = 15;	feederPeriferal[5] = 12;	feederPeriferal[6] = 10;	feederPeriferal[7] = 8;	feederPeriferal[8] = 7;	feederPeriferal[9] = 6;
	feederPeriferal[10] = 6;	feederPeriferal[11] = 5;	feederPeriferal[12] = 5;	feederPeriferal[13] = 5;	feederPeriferal[14] = 4;	feederPeriferal[15] = 4;	feederPeriferal[16] = 4;	feederPeriferal[17] = 3;	feederPeriferal[18] = 3;	feederPeriferal[19] = 3;
	
	//feeder centered
	feederCentered[0] = 15;	feederCentered[1] = 15;	feederCentered[2] = 15;	feederCentered[3] = 15;	feederCentered[4] = 12;	feederCentered[5] = 10;	feederCentered[6] = 8;	feederCentered[7] = 7;	feederCentered[8] = 6;	feederCentered[9] = 5;
	feederCentered[10] = 5;	feederCentered[11] = 4;	feederCentered[12] = 4;	feederCentered[13] = 4;	feederCentered[14] = 3;	feederCentered[15] = 3;	feederCentered[16] = 3;	feederCentered[17] = 3;	feederCentered[18] = 3;	feederCentered[19] = 3;
}

void Create_camera_image()
{
	for (int8 y = 0; y < NUMBEHAVIORS; y++)
		behaviorViews[y] = gui_rectangle_new(theGuiWindow, 10, 125 + (6 * y), 100, 5, GUI_BLACK, 1);
	
	//pixmap routines
	cameraPixmap = gui_pixmap_new_from_size(K6300_WIDTH, K6300_HEIGHT);
	filterPixmap = gui_pixmap_new_from_size(K6300_WIDTH, K6300_HEIGHT);
	
	for (int8 x = 0; x < K6300_WIDTH; x++)
		for (int8 y = 0; y < K6300_HEIGHT; y++)
		{
			gui_pixmap_draw_pixel(cameraPixmap, x, y, GUI_BLACK);
			gui_pixmap_draw_pixel(filterPixmap, x, y, GUI_BLACK);
		}
	
	cameraImage = gui_image_new_from_pixmap(theGuiWindow, 10, 55, cameraPixmap);
	filterImage = gui_image_new_from_pixmap(theGuiWindow, 190, 55, filterPixmap);
}

void Paint_black_camera_image()
{
	for (int8 y = 0 ; y < K6300_HEIGHT; y++)
		for (int8 x = 0; x < K6300_WIDTH; x++)
			gui_pixmap_draw_pixel(cameraPixmap, x, y, GUI_BLACK);

	gui_image_show(cameraImage);	//redraws the image
}

void Paint_black_filter_image()
{
	for (int8 y = 0 ; y < K6300_HEIGHT; y++)
		for (int8 x = 0; x < K6300_WIDTH; x++)
			gui_pixmap_draw_pixel(filterPixmap, x, y, GUI_BLACK);

	gui_image_show(filterImage);	//redraws the image
}

void Gui_callback()
{
	//original and pixmap routines
	gui_widget w;
	if (gui_event_get_info() == GUI_WIDGET)
	{
		w = gui_event_get_widget();
		if ((w == cameraCheckBox) && !gui_checkbox_is_active(cameraCheckBox))
			Paint_black_camera_image();
		else if ((w == filterCheckBox) && !gui_checkbox_is_active(filterCheckBox))
			Paint_black_filter_image();
	}
}

void Create_gui()
{
	theGuiWindow = gui_window_new("Moods/Eye/Filter/Behaviors", 10, 20, 370, 230);
	Create_camera_image();
	cameraCheckBox = gui_checkbox_new(theGuiWindow, 10, 5, "image", true);
	filterCheckBox = gui_checkbox_new(theGuiWindow, 90, 5, "filter", true);
	
	gui_event_callback(Gui_callback);
	gui_window_show(theGuiWindow);
}

void Change_camera_image()
{
	uint8 theColor[3];
	uint32 color;
	
	for (int8 y = 0 ; y < K6300_HEIGHT ; y++)
		for (int8 x = 0 ; x < K6300_WIDTH ; x++)
		{
			theColor[RED] = k6300_get_red(rawImage, x, y);
			theColor[GREEN] = k6300_get_green(rawImage, x, y);
			theColor[BLUE] = k6300_get_blue(rawImage, x, y);
			color = gui_color_get(theColor[RED], theColor[GREEN], theColor[BLUE]);
			gui_pixmap_draw_pixel(cameraPixmap, x, y, color);
		}
		
	gui_image_show(cameraImage);
}

//Updating the gui image which results from the vision processing (not necessary for the robot, only for an observer
void Update_filter_camera()
{
	int16 x, y;
	uint32 color;
	int16 leftHalf = 22 - (leftAvoidVisualHeight / 2);
	int16 rightHalf = 22 - (rightAvoidVisualHeight / 2);
	Box feederObjectBox = insideOutBox;
	
	for (y = 0; y < K6300_HEIGHT; y++)
		for (x = 0; x < K6300_WIDTH; x++)
		{
			switch (filteredImageCodes[x][y])
			{
				case FEEDER_ON:
					color = gui_color_get(feeder_on[RED], feeder_on[GREEN], feeder_on[BLUE]);
					break;
				case FEEDER_OFF:
					color = gui_color_get(feeder_off[RED], feeder_off[GREEN], feeder_off[BLUE]);
					break;
				case FEEDERBOX:
					color = gui_color_get(feederBox[RED], feederBox[GREEN], feederBox[BLUE]);
					break;
				case GROUND:
					color = gui_color_get(ground[RED], ground[GREEN], ground[BLUE]);
					break;
				case SKY:
					color = gui_color_get(sky[RED], sky[GREEN], sky[BLUE]);
					break;
				case RED_CORNER:
					color = gui_color_get(red_corner[RED], red_corner[GREEN], red_corner[BLUE]);
					break;
				case CYAN_CORNER:
					color = gui_color_get(cyan_corner[RED], cyan_corner[GREEN], cyan_corner[BLUE]);
					break;
				case SIDEWALL:
					color = gui_color_get(sideWall[RED], sideWall[GREEN], sideWall[BLUE]);
					break;
				case WHITEBOX:
					color = gui_color_get(whiteBox[RED], whiteBox[GREEN], whiteBox[BLUE]);
					break;
				case MAGENTABOX:
					color = gui_color_get(magentaBox[RED], magentaBox[GREEN], magentaBox[BLUE]);
					break;
				case YELLOWBOX:
					color = gui_color_get(yellowBox[RED], yellowBox[GREEN], yellowBox[BLUE]);
					break;
				case CYANBOX:
					color = gui_color_get(cyanBox[RED], cyanBox[GREEN], cyanBox[BLUE]);
					break;
				case DISK:
					color = gui_color_get(disk[RED], disk[GREEN], disk[BLUE]);
					break;
				default:
					color = gui_color_get(0, 0, 0);
					break;
			}
			
			gui_pixmap_draw_pixel(filterPixmap, x, y, color);
		}
	
	//Frame the central "path" and the periferal object detectors
	color = gui_color_get(0, 0, 255);
	for (y = K6300_HEIGHT - leftAvoidVisualHeight; y < K6300_HEIGHT; y++)
	{
		if (y == K6300_HEIGHT - leftAvoidVisualHeight)
			for (x = k6300_halfWidth - leftHalf; x < k6300_halfWidth; x++)
				gui_pixmap_draw_pixel(filterPixmap, x, y, color);
		else gui_pixmap_draw_pixel(filterPixmap, k6300_halfWidth - leftHalf, y, color);
		
		if (y % 2 == 0)
			leftHalf++;
	}
	for (y = K6300_HEIGHT - rightAvoidVisualHeight; y < K6300_HEIGHT; y++)
	{
		if (y == K6300_HEIGHT - rightAvoidVisualHeight)
			for (x = k6300_halfWidth  - 1 + rightHalf; x >= k6300_halfWidth; x--)
				gui_pixmap_draw_pixel(filterPixmap, x, y, color);
		else gui_pixmap_draw_pixel(filterPixmap, k6300_halfWidth - 1 + rightHalf, y, color);
		
		if (y % 2 == 0)
			rightHalf++;
	}
	leftHalf = rightHalf = 19;
	for (y = 53; y < K6300_HEIGHT; y++)
	{
		if (y == 53)
		{
			for (x = 16; x <= 20; x++)
				gui_pixmap_draw_pixel(filterPixmap, x, y, color);
			for (x = 63; x >= 59; x--)
				gui_pixmap_draw_pixel(filterPixmap, x, y, color);
		}
		else
		{
				gui_pixmap_draw_pixel(filterPixmap, k6300_halfWidth - leftHalf - 5, y, color);
				gui_pixmap_draw_pixel(filterPixmap, k6300_halfWidth - 1 + rightHalf + 5, y, color);
		}
		
		if (y % 2 == 0)
		{
			leftHalf++;
			rightHalf++;
		}
	}
	
	if (feederOnDetected)
		feederObjectBox = feederOnBound;
	else if (feederBoxDetected && recentlyFeederOrienting <= 0)
		feederObjectBox = feederBoxBound;
	
	//if a confirmed feeder or feederBox was found, corner a red rectangle around it
	if (feederObjectBox.left <= feederObjectBox.right)
	{
		color = gui_color_get(255, 0, 0);
		
		for (y = feederObjectBox.top; y <= feederObjectBox.top + 3; y++)
		{
			if (y <= feederObjectBox.bottom)
			{
				gui_pixmap_draw_pixel(filterPixmap, feederObjectBox.left, y, color);
				gui_pixmap_draw_pixel(filterPixmap, feederObjectBox.right, y, color);
			}
		}
		
		for (y = feederObjectBox.bottom; y >= feederObjectBox.bottom - 3; y--)
		{
			if (y >= feederObjectBox.top)
			{
				gui_pixmap_draw_pixel(filterPixmap, feederObjectBox.left, y, color);
				gui_pixmap_draw_pixel(filterPixmap, feederObjectBox.right, y, color);
			}
		}
		
		for (x = feederObjectBox.left; x <= feederObjectBox.left + 3; x++)
		{
			if (x <= feederObjectBox.right)
			{
				gui_pixmap_draw_pixel(filterPixmap, x, feederObjectBox.top, color);
				gui_pixmap_draw_pixel(filterPixmap, x, feederObjectBox.bottom, color);
			}
		}
		
		for (x = feederObjectBox.right; x >= feederObjectBox.right - 3; x--)
		{
			if (x >= feederObjectBox.left)
			{
				gui_pixmap_draw_pixel(filterPixmap, x, feederObjectBox.top, color);
				gui_pixmap_draw_pixel(filterPixmap, x, feederObjectBox.bottom, color);
			}
		}
	}
	//if there is a goal landmark, corner a green rectangle around it
	else if (thePresentTimeStepData.presentLandmarkGoalBox.left != -1)
	{
		color = gui_color_get(0, 128, 0);
		
		for (y = thePresentTimeStepData.presentLandmarkGoalBox.top; y <= thePresentTimeStepData.presentLandmarkGoalBox.top + 3; y++)
		{
			if (y <= thePresentTimeStepData.presentLandmarkGoalBox.bottom)
			{
				gui_pixmap_draw_pixel(filterPixmap, thePresentTimeStepData.presentLandmarkGoalBox.left, y, color);
				gui_pixmap_draw_pixel(filterPixmap, thePresentTimeStepData.presentLandmarkGoalBox.left, y, color);
			}
		}
		
		for (y = thePresentTimeStepData.presentLandmarkGoalBox.bottom; y >= thePresentTimeStepData.presentLandmarkGoalBox.bottom - 3; y--)
		{
			if (y >= thePresentTimeStepData.presentLandmarkGoalBox.top)
			{
				gui_pixmap_draw_pixel(filterPixmap, thePresentTimeStepData.presentLandmarkGoalBox.left, y, color);
				gui_pixmap_draw_pixel(filterPixmap, thePresentTimeStepData.presentLandmarkGoalBox.left, y, color);
			}
		}
		
		for (x = thePresentTimeStepData.presentLandmarkGoalBox.left; x <= thePresentTimeStepData.presentLandmarkGoalBox.left + 3; x++)
		{
			if (x <= thePresentTimeStepData.presentLandmarkGoalBox.right)
			{
				gui_pixmap_draw_pixel(filterPixmap, x, thePresentTimeStepData.presentLandmarkGoalBox.top, color);
				gui_pixmap_draw_pixel(filterPixmap, x, thePresentTimeStepData.presentLandmarkGoalBox.bottom, color);
			}
		}
		
		for (x = thePresentTimeStepData.presentLandmarkGoalBox.right; x >= thePresentTimeStepData.presentLandmarkGoalBox.right - 3; x--)
		{
			if (x >= thePresentTimeStepData.presentLandmarkGoalBox.left)
			{
				gui_pixmap_draw_pixel(filterPixmap, x, thePresentTimeStepData.presentLandmarkGoalBox.top, color);
				gui_pixmap_draw_pixel(filterPixmap, x, thePresentTimeStepData.presentLandmarkGoalBox.bottom, color);
			}
		}
	}
	
	gui_image_show(filterImage);
}

//Filter the camera image for visual features (create the filtered image)
void CreateFilteredImages()
{
	uint8 theColor[3];					//color of a camera pixel
	int16 farGround = LEFT_AND_RIGHT;	//is the farthest ground on the left or the right?
	int16 farSky = LEFT_AND_RIGHT;		//is the farthest sky on the left or the right?
	int8 preferLeftFeeder = 0;			//used to discrtiminate between two visible feeders
	int8 preferRightFeeder = 0;			//used to discrtiminate between two visible feeders
	bool leftTopPresent, leftBottomPresent, rightTopPresent, rightBottomPresent;		//used to discrtiminate between two visible feeders
	int8 checkDistance = 0;				//used to discrtiminate between two visible feeders
	int8 feederOffBorder;				//how much of a border around a feeder box should be check for red feeder pixels
	
	feederOnDetected = false;
	feederOffDetected = false;
	feederBoxDetected = false;
	groundVisible = false;
	difGroundRepulsion = 0;
	leftGroundRepulsion = rightGroundRepulsion = leftWallRepulsion = rightWallRepulsion = 0;
	skyBalance = 0;
	groundTop = k6300_heightMinus1;
	skyBottom = 0;
	feederOnDistance = feederOffDistance = 0;
	feederOnBound = insideOutBox;
	feederOffBound = insideOutBox;
	feederBoxBound = insideOutBox;
	whiteBoxBound = insideOutBox;
	magentaBoxBound = insideOutBox;
	yellowBoxBound = insideOutBox;
	cyanBoxBound = insideOutBox;
	diskBound = insideOutBox;
	redCornerBound = insideOutBox;
	cyanCornerBound = insideOutBox;
	groundValue = 0;
	
	//create a priliminary filtered view of green feeders, ground, and sky.
	for (int8 y = 0; y < K6300_HEIGHT; y++)
		for (int8 x = 0; x < K6300_WIDTH; x++)
		{
			theColor[RED]	= k6300_get_red(rawImage, x, y);
			theColor[GREEN]	= k6300_get_green(rawImage, x, y);
			theColor[BLUE]	= k6300_get_blue(rawImage, x, y);
			
			if (fabs(theColor[RED] - theColor[BLUE]) < COLOR_TOLERANCE && theColor[RED] < theColor[GREEN])
			{
				filteredImageCodes[x][y] = FEEDER_ON;
				feederOnDetected = true;
			}
			else if (y > 25 && theColor[RED] > 255 - COLOR_TOLERANCE && theColor[GREEN] > 213 - COLOR_TOLERANCE && theColor[GREEN] < 213 + COLOR_TOLERANCE && theColor[BLUE] > 123 - COLOR_TOLERANCE && theColor[BLUE] < 123 + COLOR_TOLERANCE)
			{
				filteredImageCodes[x][y] = GROUND;
				if (y < groundTop)
				{
					groundVisible = true;	//doing this inside the groundTop loop saves the trouble of testing for this on every ground pixel
					
					groundTop = y;
					if (x < k6300_halfWidth - 10)
						farGround = LEFT;
					else if (x > k6300_halfWidth + 11)
						farGround = RIGHT;
				}
				
				groundValue += (K6300_HEIGHT - y);
			}
			else if (y < 35 && theColor[RED] > 98 - COLOR_TOLERANCE && theColor[RED] < 98 + COLOR_TOLERANCE && theColor[GREEN] > 180 - COLOR_TOLERANCE && theColor[GREEN] < 180 + COLOR_TOLERANCE && theColor[BLUE] > 255 - COLOR_TOLERANCE)
			{
				filteredImageCodes[x][y] = SKY;
				if (y > skyBottom)
				{
					skyBottom = y;
					if (x < k6300_halfWidth - 10)
						farSky = LEFT;
					else if (x > k6300_halfWidth + 11)
						farSky = RIGHT;
				}
			}
			else filteredImageCodes[x][y] = BLANK;
		}
	
	if (feederOnDetected && !CheckFeederAgainstAnotherKhepera())
	{
		feederOnBound = insideOutBox;	//feeder colors were detected but it was decided that they pertain to another Khepera
									//robot and not to an actual feeder.  Therefore, we reset the min and max feeder coordinates
									//so that the Orient behavior will know it doesn't have anything to orient towards.
		feederOnDetected = false;
	}
	
	//if there are more than two green feeders visible, break them into separate objects and go after the nearer one.
	if (feederOnDetected)
	{
		if (feederOnBound.width > 10)
		{
			leftTopPresent = leftBottomPresent = rightTopPresent = rightBottomPresent = false;
			
			if (feederOnBound.width < 20)
				checkDistance = 5;
			else if (feederOnBound.width < 30)
				checkDistance = 10;
			else if (feederOnBound.width < 40)
				checkDistance = 15;
			else if (feederOnBound.width < 50)
				checkDistance = 20;
			else checkDistance = 25;
			
			if ((float)feederOnBound.width / (float)feederOnBound.height > 1.0)
			{
				for (int8 x = feederOnBound.left; x < feederOnBound.left + checkDistance; x++)
				{
					if (filteredImageCodes[x][feederOnBound.top] == FEEDER_ON)
					{
						preferLeftFeeder++;
						leftTopPresent = true;
					}
					
					if (filteredImageCodes[x][feederOnBound.bottom] == FEEDER_ON)
					{
						preferLeftFeeder++;
						leftBottomPresent = true;
					}
				}
				
				for (int8 x = feederOnBound.right; x > feederOnBound.right - checkDistance; x--)
				{
					if (filteredImageCodes[x][feederOnBound.top] == FEEDER_ON)
					{
						preferRightFeeder++;
						rightTopPresent = true;
					}
					
					if (filteredImageCodes[x][feederOnBound.bottom] == FEEDER_ON)
					{
						preferRightFeeder++;
						rightBottomPresent = true;
					}
				}
				
				if (leftTopPresent && leftBottomPresent && (!rightTopPresent || !rightBottomPresent))
				{
					//take the left feeder
					feederOnBound.right = feederOnBound.left;
					while (filteredImageCodes[feederOnBound.right + 1][(feederOnBound.top + feederOnBound.bottom) / 2] == FEEDER_ON)
						feederOnBound.right++;
				}
				else if (rightTopPresent && rightBottomPresent && (!leftTopPresent || !leftBottomPresent))
				{
					//take the right feeder
					feederOnBound.left = feederOnBound.right;
					while (filteredImageCodes[feederOnBound.left - 1][(feederOnBound.top + feederOnBound.bottom) / 2] == FEEDER_ON)
						feederOnBound.left--;
				}
				else if (preferLeftFeeder > preferRightFeeder)
				{
					//take the left feeder
					feederOnBound.right = feederOnBound.left;
					while (filteredImageCodes[feederOnBound.right + 1][(feederOnBound.top + feederOnBound.bottom) / 2] == FEEDER_ON)
						feederOnBound.right++;
				}
				else if (preferRightFeeder > preferLeftFeeder)
				{
					//take the right feeder
					feederOnBound.left = feederOnBound.right;
					while (filteredImageCodes[feederOnBound.left - 1][(feederOnBound.top + feederOnBound.bottom) / 2] == FEEDER_ON)
						feederOnBound.left--;
				}
				
				feederOnBound.width = feederOnBound.right - feederOnBound.left + 1;
				feederOnBound.height = feederOnBound.bottom - feederOnBound.top + 1;
				feederOnBound.horizontalCenter = (feederOnBound.left + feederOnBound.right) / 2;
			}
			else feederOnBound.horizontalCenter = (feederOnBound.left + feederOnBound.right) / 2;
		}
		else feederOnBound.horizontalCenter = (feederOnBound.left + feederOnBound.right) / 2;
		
		feederOnDistance = LookUpDistance(FEEDER_ON, feederOnBound);
	}
	
	if (seekingCloseFeeder && feederOnDistance > 4)
	{
		feederOnDetected = false;
		feederOnBound = insideOutBox;
	}
	
	//go through the camera image again, adding to the filtered image, but only check BLANK pixels, those not previously set to green feeders, ground, and sky.
	for (int8 y = 0; y < K6300_HEIGHT; y++)
		for (int8 x = 0; x < K6300_WIDTH; x++)
		{
			if (filteredImageCodes[x][y] == BLANK)
			{
				
				theColor[RED]	= k6300_get_red(rawImage, x, y);
				theColor[GREEN]	= k6300_get_green(rawImage, x, y);
				theColor[BLUE]	= k6300_get_blue(rawImage, x, y);
				
				if (fabs(theColor[GREEN] - theColor[BLUE]) < COLOR_TOLERANCE && theColor[RED] > theColor[GREEN])
				{
					filteredImageCodes[x][y] = FEEDER_OFF;
					feederOffDetected = true;
					
					if (x < feederOffBound.left)
						feederOffBound.left = x;
					if (x > feederOffBound.right)
						feederOffBound.right = x;
					if (y < feederOffBound.top)
						feederOffBound.top = y;
					if (y > feederOffBound.bottom)
						feederOffBound.bottom = y;
				}
				else if (fabs(theColor[RED] - theColor[GREEN]) < COLOR_TOLERANCE && fabs(theColor[GREEN] - theColor[BLUE]) < COLOR_TOLERANCE)
				{
					filteredImageCodes[x][y] = WHITEBOX;
					
					if (x < whiteBoxBound.left)
						whiteBoxBound.left = x;
					if (x > whiteBoxBound.right)
						whiteBoxBound.right = x;
					if (y < whiteBoxBound.top)
						whiteBoxBound.top = y;
					if (y > whiteBoxBound.bottom)
						whiteBoxBound.bottom = y;
				}
				else if (fabs(theColor[RED] - theColor[BLUE]) < COLOR_TOLERANCE && theColor[RED] > theColor[GREEN] && (float)theColor[RED] / (float)theColor[GREEN] > 1.95)
				{
					filteredImageCodes[x][y] = MAGENTABOX;
					
					if (x < magentaBoxBound.left)
						magentaBoxBound.left = x;
					if (x > magentaBoxBound.right)
						magentaBoxBound.right = x;
					if (y < magentaBoxBound.top)
						magentaBoxBound.top = y;
					if (y > magentaBoxBound.bottom)
						magentaBoxBound.bottom = y;
				}
				//else if (theColor[RED] > theColor[GREEN] && theColor[GREEN] > theColor[BLUE] && theColor[BLUE] > COLOR_TOLERANCE && theColor[BLUE] != 0 && theColor[RED] / theColor[BLUE] >= 7)
				else if (theColor[GREEN] > theColor[BLUE] && theColor[BLUE] > COLOR_TOLERANCE && theColor[BLUE] != 0 && theColor[RED] / theColor[BLUE] >= 7)
				{
					filteredImageCodes[x][y] = YELLOWBOX;
					
					if (x < yellowBoxBound.left)
						yellowBoxBound.left = x;
					if (x > yellowBoxBound.right)
						yellowBoxBound.right = x;
					if (y < yellowBoxBound.top)
						yellowBoxBound.top = y;
					if (y > yellowBoxBound.bottom)
						yellowBoxBound.bottom = y;
				}
				else if (theColor[GREEN] > theColor[BLUE] && theColor[BLUE] > theColor[RED] && theColor[RED] > COLOR_TOLERANCE)
				{
					filteredImageCodes[x][y] = CYANBOX;
					
					if (x < cyanBoxBound.left)
						cyanBoxBound.left = x;
					if (x > cyanBoxBound.right)
						cyanBoxBound.right = x;
					if (y < cyanBoxBound.top)
						cyanBoxBound.top = y;
					if (y > cyanBoxBound.bottom)
						cyanBoxBound.bottom = y;
				}
				else if (theColor[RED] > theColor[BLUE] && theColor[BLUE] > theColor[GREEN])
				{
					filteredImageCodes[x][y] = RED_CORNER;
					
					if (x < redCornerBound.left)
						redCornerBound.left = x;
					if (x > redCornerBound.right)
						redCornerBound.right = x;
					if (y < redCornerBound.top)
						redCornerBound.top = y;
					if (y > redCornerBound.bottom)
						redCornerBound.bottom = y;
				}
				else if (fabs(theColor[GREEN] - theColor[BLUE]) < COLOR_TOLERANCE && theColor[RED] < COLOR_TOLERANCE)
				{
					filteredImageCodes[x][y] = CYAN_CORNER;
					
					if (x < cyanCornerBound.left)
						cyanCornerBound.left = x;
					if (x > cyanCornerBound.right)
						cyanCornerBound.right = x;
					if (y < cyanCornerBound.top)
						cyanCornerBound.top = y;
					if (y > cyanCornerBound.bottom)
						cyanCornerBound.bottom = y;
				}
				else if (fabs(theColor[RED] - theColor[BLUE]) < COLOR_TOLERANCE && theColor[RED] > theColor[GREEN] && (float)theColor[RED] / (float)theColor[GREEN] < 1.95)
				{
					filteredImageCodes[x][y] = DISK;
					
					if (x < diskBound.left)
						diskBound.left = x;
					if (x > diskBound.right)
						diskBound.right = x;
					if (y < diskBound.top)
						diskBound.top = y;
					if (y > diskBound.bottom)
						diskBound.bottom = y;
				}
				else if (!seekingFeeder && fabs(theColor[RED] - theColor[GREEN]) < COLOR_TOLERANCE && theColor[RED] < theColor[BLUE] && theColor[BLUE] > 164 - COLOR_TOLERANCE && theColor[RED] > 115 - COLOR_TOLERANCE)
				{
					filteredImageCodes[x][y] = FEEDERBOX;
					feederBoxDetected = true;
					
					if (x < feederBoxBound.left)
						feederBoxBound.left = x;
					if (x > feederBoxBound.right)
						feederBoxBound.right = x;
					if (y < feederBoxBound.top)
						feederBoxBound.top = y;
					if (y > feederBoxBound.bottom)
						feederBoxBound.bottom = y;
				}
				else if (fabs(theColor[RED] - theColor[GREEN]) < COLOR_TOLERANCE && theColor[RED] < theColor[BLUE] && theColor[RED] != 0 && theColor[BLUE] / theColor[RED] >= 3)
					filteredImageCodes[x][y] = SIDEWALL;
			}
		}
	
	//if there are more than two red feeders visible, break them into separate objects, prefering the nearer one.
	if (feederOffDetected)
	{
		if (feederOffBound.width > 10)
		{
			leftTopPresent = leftBottomPresent = rightTopPresent = rightBottomPresent = false;
			
			if (feederOffBound.width < 20)
				checkDistance = 5;
			else if (feederOffBound.width < 30)
				checkDistance = 10;
			else if (feederOffBound.width < 40)
				checkDistance = 15;
			else if (feederOffBound.width < 50)
				checkDistance = 20;
			else checkDistance = 25;
			
			if ((float)feederOffBound.width / (float)feederOffBound.height > 1.0)
			{
				for (int8 x = feederOffBound.left; x < feederOffBound.left + checkDistance; x++)
				{
					if (filteredImageCodes[x][feederOffBound.top] == FEEDER_OFF)
					{
						preferLeftFeeder++;
						leftTopPresent = true;
					}
					
					if (filteredImageCodes[x][feederOffBound.bottom] == FEEDER_OFF)
					{
						preferLeftFeeder++;
						leftBottomPresent = true;
					}
				}
				
				for (int8 x = feederOffBound.right; x > feederOffBound.right - checkDistance; x--)
				{
					if (filteredImageCodes[x][feederOffBound.top] == FEEDER_OFF)
					{
						preferRightFeeder++;
						rightTopPresent = true;
					}
					
					if (filteredImageCodes[x][feederOffBound.bottom] == FEEDER_OFF)
					{
						preferRightFeeder++;
						rightBottomPresent = true;
					}
				}
				
				if (leftTopPresent && leftBottomPresent && (!rightTopPresent || !rightBottomPresent))
				{
					//take the left feeder
					feederOffBound.right = feederOffBound.left;
					while (filteredImageCodes[feederOffBound.right + 1][(feederOffBound.top + feederOffBound.bottom) / 2] == FEEDER_OFF)
						feederOffBound.right++;
				}
				else if (rightTopPresent && rightBottomPresent && (!leftTopPresent || !leftBottomPresent))
				{
					//take the right feeder
					feederOffBound.left = feederOffBound.right;
					while (filteredImageCodes[feederOffBound.left - 1][(feederOffBound.top + feederOffBound.bottom) / 2] == FEEDER_OFF)
						feederOffBound.left--;
				}
				else if (preferLeftFeeder > preferRightFeeder)
				{
					//take the left feeder
					feederOffBound.right = feederOffBound.left;
					while (filteredImageCodes[feederOffBound.right + 1][(feederOffBound.top + feederOffBound.bottom) / 2] == FEEDER_OFF)
						feederOffBound.right++;
				}
				else if (preferRightFeeder > preferLeftFeeder)
				{
					//take the right feeder
					feederOffBound.left = feederOffBound.right;
					while (filteredImageCodes[feederOffBound.left - 1][(feederOffBound.top + feederOffBound.bottom) / 2] == FEEDER_OFF)
						feederOffBound.left--;
				}
				
				feederOffBound.width = feederOffBound.right - feederOffBound.left + 1;
				feederOffBound.height = feederOffBound.bottom - feederOffBound.top + 1;
				feederOffBound.horizontalCenter = (feederOffBound.left + feederOffBound.right) / 2;
			}
			else feederOffBound.horizontalCenter = (feederOffBound.left + feederOffBound.right) / 2;
		}
		else feederOffBound.horizontalCenter = (feederOffBound.left + feederOffBound.right) / 2;
		
		feederOffDistance = LookUpDistance(FEEDER_OFF, feederOffBound);
	}
	
	//if the feederBox rectangle has FEEDER_OFF pixels inside or around it, then that feeder is of no interest to the robot
	if (feederBoxDetected)
	{
		feederBoxBound.width = feederBoxBound.right - feederBoxBound.left + 1;
		feederBoxBound.height = feederBoxBound.bottom - feederBoxBound.top + 1;
		
		if (feederBoxBound.height < 10)
			feederOffBorder = 4;
		else if (feederBoxBound.height < 20)
			feederOffBorder = 5;
		else if (feederBoxBound.height < 30)
			feederOffBorder = 6;
		else if (feederBoxBound.height < 40)
			feederOffBorder = 7;
		else feederOffBorder = 8;
		
		for (int8 y = feederBoxBound.top; y <= feederBoxBound.bottom; y++)
			for (int8 x = feederBoxBound.left - feederOffBorder; x <= feederBoxBound.right + feederOffBorder; x++)
				if (x >= 0 && x < K6300_WIDTH && y >= 0 && y < K6300_HEIGHT)
					if (filteredImageCodes[x][y] == FEEDER_OFF)
					{
						feederBoxBound = insideOutBox;
						feederBoxDetected = false;
						
						x = K6300_WIDTH;
						y = K6300_HEIGHT;
					}
	}
	
	if (!feederOnDetected && feederBoxDetected)
	{
		feederOnBound = feederBoxBound;
		feederOnBound.height -= (feederOnBound.height / 5);
		feederOnDistance = LookUpDistance(FEEDER_ON, feederOnBound);
		
		//if the feederbox is really nearby and the robot has recently fed, then this feeder is of no interest
		//similarly, if the robot lost sight of a green feeder, it shouldn't have any interest in a feeder box
		if (recentlyFeederOrienting > 0 || (recentlyFed > 0 && feederOnDistance <= 5))
		{
			feederOnBound = insideOutBox;
			feederOnDistance = 0;
		}
	}
	
	feederOffBound.height = feederOffBound.bottom - feederOffBound.top + 1;
	if (feederOffBound.height  > 30)
		nearFeederOff = true;
	else nearFeederOff = false;
	
	//create left and right repulsions based on the left/right balance of the ground pixels
	for (int8 y = 30; y < K6300_HEIGHT; y++)
		for (int8 x = 0; x < k6300_halfWidth; x++)
		{
			if (filteredImageCodes[x][y] != GROUND)
				leftGroundRepulsion++;
			if (filteredImageCodes[x + k6300_halfWidth][y] != GROUND)
				rightGroundRepulsion++;
		}
	if (farGround == LEFT)
		rightGroundRepulsion += 50;
	else leftGroundRepulsion += 50;
		
	difGroundRepulsion = leftGroundRepulsion - rightGroundRepulsion;
	
	//create yet more repulsion against the sidewalls of the arena
	for (int8 y = 0; y < K6300_HEIGHT; y++)
		for (int8 x = 0; x < k6300_halfWidth; x++)
		{
			if (filteredImageCodes[x][y] == SIDEWALL)
				leftWallRepulsion++;
			if (filteredImageCodes[x + k6300_halfWidth][y] == SIDEWALL)
				rightWallRepulsion++;
		}
		
	difWallRepulsion = leftWallRepulsion - rightWallRepulsion;
	
	//determine the sky balance
	for (int8 y = 0; y < 30; y++)
	{
		for (int8 x = 0; x < (k6300_halfWidth / 2); x++)
		{
			if (filteredImageCodes[x][y] == SKY)
				skyBalance -= 2;
			if (filteredImageCodes[x + k6300_halfWidth][y] == SKY)
				skyBalance++;
		}
		
		for (int8 x = (k6300_halfWidth / 2); x < k6300_halfWidth; x++)
		{
			if (filteredImageCodes[x][y] == SKY)
				skyBalance--;
			if (filteredImageCodes[x + k6300_halfWidth][y] == SKY)
				skyBalance += 2;
		}
	}
	if (farSky == LEFT)
		skyBalance -= 200;
	else skyBalance += 200;
	
	IdentifyImmediateLandmarks();
	
	if (cpuCost < 20)
		oldLandmarkGoalAge = 2500;
	else if (cpuCost < 40)
		oldLandmarkGoalAge = 1800;
	else if (cpuCost < 60)
		oldLandmarkGoalAge = 1100;
	else if (cpuCost < 80)
		oldLandmarkGoalAge = 500;
	else oldLandmarkGoalAge = 250;
	
	//if there are any landmarks that haven't been the goal in a very long time, kill any visible feederBox so the landmark will become the goal
	if (!feederOnDetected && feederBoxDetected)
		for (int8 i = 0; i < theImmediateViewKnowledge.numImmediateLandmarks; i++)
		{
			if (theWorldKnowledge.timeSinceLastGoal[theImmediateViewKnowledge.visibleLandmarks[i] - 8] > oldLandmarkGoalAge)
			{
				feederBoxDetected = feederOnDetected = false;
				feederBoxBound = feederOnBound = insideOutBox;
				feederOnDistance = 0;
				
				initFeederOrient = true;
				
				i = 99;
			}
		}
}

//returns true if it thinks the green patch is a feeder.  Returns false if it
//thinks the green patch isn't which means it must be another Khepera robot.
bool CheckFeederAgainstAnotherKhepera()
{
	for (int8 y = 0; y < K6300_HEIGHT; y++)
		for (int8 x = 0; x < K6300_WIDTH; x++)
		{
			if (filteredImageCodes[x][y] == FEEDER_ON)
			{
				if (y < feederOnBound.top)
					feederOnBound.top = y;
				
				if (y <= 30)						
				{
					//We want the bounding X coordinates to represent feeders, not Kheperas.
					//That way when we zero in on the horizontal center of the rectangle it will
					//be the horizontal center of the feeder and not everything green.
					if (x < feederOnBound.left)
						feederOnBound.left = x;
					if (x > feederOnBound.right)
						feederOnBound.right = x;
				}
			}
		}
	
	//We only want the feederOnBound.bottom constraining the expected feeder.  Other Khepera data might mess this up, so we
	//only count green pixels within the proper horizontal range.  This could still be incorrect if a Khepera
	//is directly in front of a feeder, but not if the two are separated horizontally within the field of view.
	for (int8 y = feederOnBound.top; y < K6300_HEIGHT; y++)
		for (int8 x = feederOnBound.left; x <= feederOnBound.right; x++)
			if (filteredImageCodes[x][y] == FEEDER_ON && y > feederOnBound.bottom)
				feederOnBound.bottom = y;
	
	feederOnBound.width = feederOnBound.right - feederOnBound.left + 1;
	feederOnBound.height = feederOnBound.bottom - feederOnBound.top + 1;
	
	//If feederOnBound.top <= 30 then it is too high to be another Khepera and must therefore be a feeder.
	//Likewise, if feederOnBound.top > 30 then it is too low to be a feeder and must therefore be another Khepera robot.
	if (feederOnBound.top <= 30)
		return true;		//a feeder
	else return false;		//another Khepera robot
}

//orients the robot towards a feeder's center
int16 LookForFeederHorizCenter()
{
	int16 bias = 0;
	Box theBox;
	
	if (feederOnBound.right < feederOnBound.left)
	{
		if (nearFeederOff)
			return 8888;		//The robot probably just fed
		else if (feederBoxBound.right < feederBoxBound.left)
			return 9999;		//no feeders were found so there is nothing to orient towards
		else theBox = feederBoxBound;
	}
	else theBox = feederOnBound;
	
	//If the feeder is at a sharp angle then detecting this will help orient the robot more accurately.
	//Essentially this makes the robot pass by the feeder instead of bumping into it at a sharp angle
	//that could otherwise result in the robot failing to pass in front of the feeder.
	if (feederOnBound.right >= feederOnBound.left && feederOnBound.height > 16)
	{
		int16 leftHeight = 0;
		int16 rightHeight = 0;
		
		for (int8 y = feederOnBound.top; y <= feederOnBound.bottom; y++)
		{
			if (filteredImageCodes[feederOnBound.left][y] == FEEDER_ON)
				leftHeight++;
			if (filteredImageCodes[feederOnBound.right][y] == FEEDER_ON)
				rightHeight++;
		}
		
		if (leftHeight - rightHeight > 0 && leftHeight - rightHeight < 10)
			bias = (leftHeight - rightHeight) * -4;		//-(10 - (leftHeight - rightHeight));
		else if (rightHeight - leftHeight > 0 && rightHeight - leftHeight < 10)
			bias = (rightHeight - leftHeight) * 4;		//(10 - (rightHeight - leftHeight));
	}
	
	int16 offset, halfWidth;
	halfWidth = theBox.width / 2;
	offset = theBox.left + halfWidth - k6300_halfWidth - bias;
	
	return offset;
}

//orients the robot towards the landmark goal's center
int16 LookForLandmarkHorizCenter()
{
	int16 index = -1;
	
	for (int8 i = 0; i < theImmediateViewKnowledge.numImmediateLandmarks; i++)
	{
		if (theImmediateViewKnowledge.visibleLandmarks[i] == thePresentTimeStepData.presentLandmarkGoal)
		{
			index = i;
		
			i = theImmediateViewKnowledge.numImmediateLandmarks;
		}
	}
	
	if (index == -1)
		return 9999;
	
	int16 offset, halfWidth;
	halfWidth = theImmediateViewKnowledge.landmarkBoxes[index].width / 2;
	offset = theImmediateViewKnowledge.landmarkBoxes[index].left + halfWidth - k6300_halfWidth;
	
	return offset;
}

//Used to retry a glance that saw nothing but a wall after glancing.
void SetNeedToTryAgain(int16 direction)
{
	bool decision;
	
	if (filteredImageCodes[40][50] != GROUND)
		decision = true;
	else decision = false;
	
	if (direction == LEFT)
		tryLeftGlanceAgain = decision;
	else tryRightGlanceAgain = decision;
}

//Go through the filtered image looking for landmarks and create an array of the detected landmarks along with their bounding boxes
void IdentifyImmediateLandmarks()
{
	int8 x = 0;
	int8 y = 0;
	Box boundBox;
	bool foundIt;
	uint8 theColor[3];
	
	theImmediateViewKnowledge.numImmediateLandmarks = 0;

	for (int8 whichLandmark = WHITEBOX; whichLandmark <= CYAN_CORNER; whichLandmark++)
	{
		boundBox = insideOutBox;
		
		foundIt = false;
		
		switch (whichLandmark)
		{
			case WHITEBOX:
				boundBox = whiteBoxBound;
				break;
			case MAGENTABOX:
				boundBox = magentaBoxBound;
				break;
			case YELLOWBOX:
				boundBox = yellowBoxBound;
				break;
			case CYANBOX:
				boundBox = cyanBoxBound;
				break;
			case DISK:
				boundBox = diskBound;
				break;
			case RED_CORNER:
				boundBox = redCornerBound;
				break;
			case CYAN_CORNER:
				boundBox = cyanCornerBound;
				break;
		}
		
		if (boundBox.left <= boundBox.right)
		{
			foundIt = true;
			
			boundBox.width = boundBox.right - boundBox.left + 1;
			boundBox.height = boundBox.bottom - boundBox.top + 1;
		}
		
		//Distinguish between the white box and the camera of the other Khepera robot.
		if (whichLandmark == WHITEBOX && foundIt)
		{
			for (int8 y = boundBox.top; y <= boundBox.bottom + 5; y++)
				for (int8 x = boundBox.left; x <= boundBox.right; x++)
					if (y < K6300_HEIGHT)
					{
						theColor[RED]	= k6300_get_red(rawImage, x, y);
						theColor[GREEN]	= k6300_get_green(rawImage, x, y);
						theColor[BLUE]	= k6300_get_blue(rawImage, x, y);
						
						if (fabs(theColor[RED] - theColor[BLUE]) < COLOR_TOLERANCE && theColor[RED] < theColor[GREEN])
						{
							foundIt = false;	//The white block that was detected was actually the camera on top of another Khepera robot.
							x = boundBox.right + 1;
							y = boundBox.bottom + 8;
						}
					}
					else y = boundBox.bottom + 8;
		}
		
		//Distinguish between the yellow box and the yellow corners.
		if (whichLandmark == YELLOWBOX && foundIt)
			if (boundBox.top < 31)
				foundIt = false;
		
		if (foundIt)
		{
			theImmediateViewKnowledge.visibleLandmarks[theImmediateViewKnowledge.numImmediateLandmarks] = whichLandmark;
			theImmediateViewKnowledge.landmarkBoxes[theImmediateViewKnowledge.numImmediateLandmarks] = boundBox;
			theImmediateViewKnowledge.landmarkDistances[theImmediateViewKnowledge.numImmediateLandmarks] = LookUpDistance(whichLandmark, boundBox);
			
			theImmediateViewKnowledge.numImmediateLandmarks++;
			
			if (theImmediateViewKnowledge.numImmediateLandmarks == MAXIMMEDIATELANDMARKS)
				whichLandmark = 9999;
		}
	}
}

//uses lookup tables to determine the distance to an object based on its type and variables concerning its bounding box
int8 LookUpDistance(int8 objectType, Box boundBox)
{
	if (objectType == FEEDER_ON || objectType == FEEDER_OFF)	//feeder
	{
		if (boundBox.height < 19)
		{
			if (boundBox.horizontalCenter < 10 || boundBox.horizontalCenter > 70)
				return feederPeriferal[boundBox.height];
			else if (boundBox.horizontalCenter > 25 && boundBox.horizontalCenter < 55)
				return feederCentered[boundBox.height];
			else return (feederPeriferal[boundBox.height] + feederCentered[boundBox.height]) / 2;
		}
		else return 3;
	}
	else if (objectType < DISK)									//box
	{
		if (boundBox.height < 25)
		{
			if (boundBox.horizontalCenter < 10 || boundBox.horizontalCenter > 70)
				return boxPeriferal[boundBox.height];
			else if (boundBox.horizontalCenter > 25 && boundBox.horizontalCenter < 55)
				return boxCentered[boundBox.height];
			else return (boxPeriferal[boundBox.height] + boxCentered[boundBox.height]) / 2;
		}
		else return 3;
	}
	else if (objectType == DISK)								//disk
	{
		if (boundBox.height < 17)
		{
			if (boundBox.horizontalCenter < 10 || boundBox.horizontalCenter > 70)
				return diskPeriferal[boundBox.height];
			else if (boundBox.horizontalCenter > 25 && boundBox.horizontalCenter < 55)
				return diskCentered[boundBox.height];
			else return (diskPeriferal[boundBox.height] + diskCentered[boundBox.height]) / 2;
		}
		else return 3;
	}
	else														//corner
	{
		if (boundBox.height < 12)
		{
			if (boundBox.horizontalCenter < 10 || boundBox.horizontalCenter > 70)
				return cornerPeriferal[boundBox.width];
			else if (boundBox.horizontalCenter > 25 && boundBox.horizontalCenter < 55)
				return cornerCentered[boundBox.width];
			else return (cornerPeriferal[boundBox.width] + cornerCentered[boundBox.width]) / 2;
		}
		else return 3;
	}
	
	return 0;
}

//updates distances from visible objects to the present (passed in) object
void UpdateLandmarkDistanceRelationships(int8 objectType)
{
	if (objectType != FEEDER_ON)
	{
		//update landmark separation distances
		for (int8 i = 0; i < theImmediateViewKnowledge.numImmediateLandmarks; i++)
			if (theImmediateViewKnowledge.landmarkDistances[i] < theWorldKnowledge.landmarkSeparationDistances[objectType - 8]
					[theImmediateViewKnowledge.visibleLandmarks[i] - 8])
				theWorldKnowledge.landmarkSeparationDistances[objectType - 8][theImmediateViewKnowledge.visibleLandmarks[i] - 8] =
					theImmediateViewKnowledge.landmarkDistances[i];
		
		//update landmark separation angles
		for (int8 i = 0; i < theImmediateViewKnowledge.numImmediateLandmarks; i++)
			for (int8 j = 0; j < theImmediateViewKnowledge.numImmediateLandmarks; j++)
				if (i != j)
					theWorldKnowledge.landmarkSeparationAngles[objectType - 8][theImmediateViewKnowledge.visibleLandmarks[i] - 8]
						[theImmediateViewKnowledge.visibleLandmarks[j] - 8] =
							theImmediateViewKnowledge.landmarkBoxes[i].horizontalCenter - theImmediateViewKnowledge.landmarkBoxes[j].horizontalCenter;
		
		//update nearest feeder distances
		if (feederOnDetected && feederOnDistance < theWorldKnowledge.feederDistances[objectType - 8])
				theWorldKnowledge.feederDistances[objectType - 8] = feederOnDistance;
		
		//update nearest feeder distances
		if (feederOffDetected && feederOffDistance < theWorldKnowledge.feederDistances[objectType - 8])
				theWorldKnowledge.feederDistances[objectType - 8] = feederOffDistance;
	}
}

//detects if a wall or other obstruction is visible to the left
//used by glanceLeft
bool FindWallOnLeft()
{
	static const int16 k6300_heightMinus15 = K6300_HEIGHT - 15;
	static const int16 k6300_heightMinus25 = K6300_HEIGHT - 25;
	
	int16 width = 10;
	bool objectPresent = true;
	
	for (int8 y = k6300_heightMinus15; y > k6300_heightMinus25; y--)
		if (filteredImageCodes[0][y] == GROUND)
				objectPresent = false;
	
	return objectPresent;
}

//detects if a wall or other obstruction is visible to the right
//used by glanceRight
bool FindWallOnRight()
{
	static const int16 k6300_heightMinus15 = K6300_HEIGHT - 15;
	static const int16 k6300_heightMinus25 = K6300_HEIGHT - 25;
	
	int16 width = 10;
	bool objectPresent = true;
	
	for (int8 y = k6300_heightMinus15; y > k6300_heightMinus25; y--)
		if (filteredImageCodes[k6300_widthMinus1][y] == GROUND)
				objectPresent = false;
	
	return objectPresent;
}

main()
{
	TheKhepera khep;
	
	khepera_live();
	Create_gui();
	khepera_enable_proximity(IR_ALL_SENSORS);
	k6300_enable();
	
	InitializeData();
	
	khepera_fair_step();
	for (;;)
	{
		khep.CalculateOneStep();	//Runs the Khepera class and changes the motor speeds
		
		if (gui_checkbox_is_active(cameraCheckBox))
			Change_camera_image();
		
		if (gui_checkbox_is_active(filterCheckBox))
			Update_filter_camera();
		
		cpuCost = khepera_fair_step();
	}
}