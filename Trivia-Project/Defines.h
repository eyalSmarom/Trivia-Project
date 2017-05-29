#pragma once

// Server-Client Defines

#define Sign_In_Response 102
	#define Sign_In_Success 0
	#define Sign_In_Wrong_Details 1
	#define Sign_In_Already_Connected 2
	#define Sign_In_Fail 3

#define Sign_Up_Response 104
	#define Sign_Up_Success 0
	#define Sign_Up_Pass_Illegal 1
	#define Sign_Up_Username_Exists 2
	#define Sign_Up_Username_Illegal 3
	#define Sign_Up_Other 4

#define All_Rooms_Response 106
#define All_Rooms_Users_Response 108

#define Join_Room_Response 110
	#define Join_Room_Success 0
	#define Join_Room_Fail_Full 1
	#define Join_Room_Fail_NotExist 2

#define Leave_Room_Response 112

#define Create_Room_Response 114
	#define Create_Room_Success 0
	#define Create_Room_Fail 1

#define Close_Room_Response 116
#define Send_Question_With_Answers 118
#define True_False_Response 120
#define End_Game_Response 121
#define Best_Scores_Response 124
#define Personal_State_Response 126

// Client-Server Defines

#define Sign_In_Request 200
#define Sign_Out_Request 201
#define Sign_Up_Request 203
#define All_Rooms_List_Request 205
#define All_Room_Users_Request 207
#define Join_Room_Request 209
#define Leave_Room_Request 211
#define Create_Room_Request 213
#define Close_Room_Request 215
#define Start_Game_Request 217
#define Client_Answer 219
#define Leave_Game_Request 222
#define Best_Scores_Request 223
#define Personal_State_Request 225
#define Leave_App_Request 299

// Socket Messages Defines

#define One_Byte_Int_Num 1
#define Two_Bytes_Int_Num 2
#define Three_Bytes_Int_Num 3

// DataBase Defines

#define RETURN_IF_INVALID		       if(_sqldb == nullptr) return;
#define RETURN_RES_IF_INVALID(res)     if(_sqldb == nullptr) return res;