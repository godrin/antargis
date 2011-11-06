/*
%{
PAntEntity SWIG_RB2AntEntity(VALUE x) {
	PAntEntity e=0;
	Data_Get_Struct(x,AntEntity,e);
	return e;
}
VALUE SWIG_AntEntity2RB(PAntEntity e ) {
	return AG_NewPointerObj(e,SWIGTYPE_p_AntEntity,0);
}
bool SWIG_ANTENTITY_P(VALUE x) {
	return true; // FIXME ???
    //return TYPE(x) == T_STRING;
}

%}

namespace std
{
	specialize_std_vector(PAntEntity,SWIG_ANTENTITY_P,SWIG_RB2AntEntity,SWIG_AntEntity2RB);
	specialize_std_list(PAntEntity,SWIG_ANTENTITY_P,SWIG_RB2AntEntity,SWIG_AntEntity2RB);
}
*/


//%template(EntityPVector2) std::vector<PAntEntity>;
%template(EntityPVector) std::vector<AntEntity*>;
%template(EntityPList) std::list<AntEntity*>;
//%template(EntityVector) std::vector<AntEntityPtr>;
%template(ResourceMap) std::map<std::string,float>;
%template(AGResourceMap) std::map<AGString,float>;
%template(AGVector2Pair) std::pair<AGVector2,AGVector2>;
