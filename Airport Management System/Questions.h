/*
1. האם לשים פעמיים fp בשביל לשמור על האובייקט ? 
2. איפה אפשר להכניס את CHECK_MSG_RETURN_0?
3. מה הולך עם הביטים ?? הסתבכנו לגמרי.
CHECK_RETURN_0 - 3 // A.L 3 VVV
CHECK_RETURN_NULL - 3 // A.L 3 VVV
CHECK_MSG_RETURN_0 - 0 // A.L 3
CHECK_0_MSG_CLOSE_FILE - 1 // A.L 1 VVV
CHECK_NULL__MSG_CLOSE_FILE - 6 // A.L 1 VVV
MSG_CLOSE_RETURN_0 - 1 // A.L 1 VVV
*/