
enum SignalSet {
	kStart = 111, kStop = 222, kPause = 333,
	kContinue = 444, kUnknow = 555
};

int start_flag = 0;
int pause_flag = 0;
int  stop_flag = 0;

bool IsIrrelevant(SignalSet signal)
{
	if ((signal == kStart) && (start_flag == 1 && pause_flag == 0)) 
		return true;
	if ((signal == kPause) && (start_flag == 0 && pause_flag == 0))
		return true;
	if ((signal == kPause) && (start_flag == 1 && pause_flag == 1))
		return true;
	if ((signal == kStop) && (start_flag == 0 && pause_flag == 0))
		return true;
	if ((signal == kStop) && (start_flag == 0 && pause_flag == 0))
		return true;
	return false;
}