/* OpenGL canvases */
FL_EXPORT FL_OBJECT *fl_create_glcanvas(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT FL_OBJECT *fl_add_glcanvas(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT void fl_set_glcanvas_defaults(
		const int *config
		);

FL_EXPORT void fl_get_glcanvas_defaults(
		int config[]
		);

FL_EXPORT void fl_set_glcanvas_attributes(
		FL_OBJECT *ob,
		const int *config
		);

FL_EXPORT void fl_get_glcanvas_attributes(
		FL_OBJECT *ob,
		int *attributes
		);

FL_EXPORT void fl_set_glcanvas_direct(
		FL_OBJECT *ob,
		int direct
		);

FL_EXPORT void fl_activate_glcanvas(
		FL_OBJECT *ob
		);

FL_EXPORT XVisualInfo *fl_get_glcanvas_xvisualinfo(
		FL_OBJECT *ob
		);



#if defined(__GLX_glx_h__) || defined(GLX_H)
FL_EXPORT GLXContext fl_get_glcanvas_context(
		FL_OBJECT *ob
		);

FL_EXPORT Window fl_glwincreate(
		int *config,
		GLXContext *context,
		int w,
		int h
		);

FL_EXPORT Window fl_glwinopen(
		int *config,
		GLXContext *context,
		int w,
		int h
		);
