mapping data = ([ ]);
#define D_FILE  "etc/area"

reset(arg) {
  if (arg) return;
  restore_object(D_FILE);
  if (!mappingp(data["closed_areas"]))
    data["closed_areas"] = ([ ]);
}

security_check() {
  return file_name(previous_object())[0..10] != "obj/wiztool";
}
  
_query_closed(str) {
  int i;
  mixed res;
  if (str == "/" && data["closed_areas"]["/"]) 
    return "/";
  res = explode(str,"/");
  if (!pointerp(res))
    return 0;
  res -= ({""});
  str = "/";
  for (i=0;i<sizeof(res);i++)
    if (data["closed_areas"][str])
      return str;
    else
      str += (res[i] + "/");
  if (data["closed_areas"][str])  
    return str;
  return 0;
}

query_open(str) {
  return !_query_closed(str);
}

query_closed(str) {
  return !!_query_closed(str);
}

check_data(str) 
{
  if (!stringp(str) || !strlen(str))
    return 0;
    
  if (str[strlen(str)-1] != '/')
    str = str + "/";

  if (str[0] != '/')
    str = "/"+ str;

  return str;
}

add_closed_area(str) 
{
  mixed tmp;
  
  if (security_check())
    return 0;
  if (!(str = check_data(str)))
    return 0;

  if (data["closed_areas"][str]) 
    return -1;

  if (tmp = _query_closed(str)) {
    data["closed_areas"][str] = 1;
    save_object(D_FILE);  
    return tmp;
  }
  data["closed_areas"][str] = 1;
  save_object(D_FILE);  
  return 1;
}

clear_closed_area() 
{
  if (security_check())
    return 0;
  data["closed_areas"] = ([ ]);
  save_object(D_FILE);  
}

remove_closed_area(str) 
{
  mixed tmp;
  if (security_check())
    return 0;
  if (!(str = check_data(str)))
    return 0;
  if (!data["closed_areas"][str]) 
    return -1;
  _m_delete(data["closed_areas"],str);
  save_object(D_FILE);  
  if (tmp = _query_closed(str))
    return tmp;
  return 1;
}

query_closed_areas(str) {
  if (security_check())
    return 0;
  if (str)
    return data["closed_areas"][check_data(str)];
  return data["closed_areas"];
}

print_closed_areas() {
  write(sprintf("%O",data["closed_areas"]));
}
  
int query_prevent_shadow() { return 1; }
