#define TPL_HTML_START "<html><head><title>%s %s</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\"><style>body{font-family:sans-serif;background-color:#333;color:#F0EAD6;}a{color:#99f;}</style><body><h1>%s %s</h1>"
#define TPL_HTML_END String("</body></html>")

#define TPL_ITEM "<li><a href=\"%s/download\">%s</a> <a href=\"%s/delete\" onclick=\"return confirm('%s');\">&#10060;</a></li>"

#define TPL_FORM "<form method=\"POST\" enctype=\"multipart/form-data\" action=\"/upload\">%s<br /><input type=\"file\" name=\"file\" accept=\".gpx\" /><br /><input type=\"submit\" value=\"%s\"></form>"
#define TPL_SEP String("<hr />")
