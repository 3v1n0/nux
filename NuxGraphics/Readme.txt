
Using GLSL texture:
-------------------
	// Get the sampler uniform location.
	GLint BumpMapLoc = glGetUniformLocationARB(prog, "BumpTex"); 

	// Bind the texture to texture unit i.
	glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(GL_TEXTURE_2D, texture_gl_object_id);

	// Pass i as an integer by glUniform. 
	glUniform1iARB(BumpMapLoc, i);


GLSL Built-in vertex attribute(Deprecated in GLSL 1.30)
-------------------------------
    Built-in vertex attribute name      Incompatible aliased vertex attribute index
    gl_Vertex                           0
    gl_Normal                           2
    gl_Color                            3
    gl_SecondaryColor                   4
    gl_FogCoord                         5
    gl_MultiTexCoord0                   8
    gl_MultiTexCoord1                   9
    gl_MultiTexCoord2                   10
    gl_MultiTexCoord3                   11
    gl_MultiTexCoord4                   12
    gl_MultiTexCoord5                   13
    gl_MultiTexCoord6                   14
    gl_MultiTexCoord7                   15


    gl_Vertex:
        - glVertex{234sifd}
        - glEnableClientState/glDisableClientState(GL_VERTEX_ARRAY); glVertexPointer(...);
    gl_Normal:
        - glNormal{3sifd}
        - glEnableClientState/glDisableClientState(GL_NORMAL_ARRAY); glNormalPointer(...);
    gl_Color:
        - glColor{34sifd}
        - glEnableClientState/glDisableClientState(GL_COLOR_ARRAY); glColorPointer(...);
    gl_SecondaryColor(requires GL_EXT_secondary_color)
        - glSecondaryColor3{bsifd};
        - glEnableClientState/glDisableClientState(SECONDARY_COLOR_ARRAY_EXT); glSecondaryColorPointerEXT(...);
    gl_FogCoord(requires GL_EXT_fog_coord)
        - glFogCoord{fd};
        - glEnableClientState/glDisableClientState(FOG_COORDINATE_ARRAY_EXT); glFogCoordPointerEXT(...);
    gl_MultiTexCoordXXX
        - glMultiTexCoord{234fd}
        - glClientActiveTextureARB(GL_TEXTUREXXX_ARB); glEnableClientState/glDisableClientState(GL_TEXTURE_COORD_ARRAY); glTexCoordPointer(...);

GLSL Vertex Shader Special output variables(write)
----------------------------------------------------
    gl_Position     (must be written to)
    gl_PointSize    (may be written to)
    gl_ClipVertex   (may be written to)

GLSL Vertex Shader Built-in varying(write)                     GLSL Fragment Shader Built-in varying(read)
-------------------------------------------                     -------------------------------------------
    varying vec4    gl_FrontColor;                     ---->    gl_Color
    varying vec4    gl_BackColor;                      ---->    gl_Color
    varying vec4    gl_FrontSecondaryColor;            ---->    gl_SecondaryColor
    varying vec4    gl_BackSecondaryColor;             ---->    gl_SecondaryColor
    varying vec4    gl_TexCoord[];                     ---->    gl_TexCoord[]
    varying float   gl_FogFragCoord;                   ---->    gl_FogFragCoord

GLSL Fragment Built-in variables
--------------------------------
    vec4    gl_FragCoord    (read only)
    bool    gl_FrontFacing  (read only)
    vec2    gl_PointCoord   (read only)

GLSL Fragment Shader Special output variables
---------------------------------------------
    vec4    gl_FragColor    (may be written to)
    vec4    gl_FragData[gl_MaxDrawBuffers]  (may be written to)
    float   gl_FragDepth    (may be written to)
    vec2    gl_PointCoord   (read only)



Binding Semantics for Cg programs
Binding Semantics for Varying Input/Output Data
-----------------------------------------------
    Table 23 summarizes the valid binding semantics for varying input parameters
    in the vp30 profile.
    	One can also use TANGENT and BINORMAL instead of TEXCOORD6 and
    	TEXCOORD7. These binding semantics map to NV_vertex_program2 input
    	attribute parameters. The two sets act as aliases to each other.
    
    Table 23 vp30 Varying Input Binding Semantics
    		Binding Semantics                           Name Corresponding Data
    		POSITION, ATTR0                             Input Vertex, Generic Attribute 0
    		BLENDWEIGHT, ATTR1                          Input vertex weight, Generic Attribute 1
    		NORMAL, ATTR2                               Input normal, Generic Attribute 2
    		COLOR0, DIFFUSE, ATTR3                      Input primary color, Generic Attribute 3
    		COLOR1, SPECULAR, ATTR4                     Input secondary color, Generic Attribute 4
    		TESSFACTOR, FOGCOORD, ATTR5                 Input fog coordinate, Generic Attribute 5
    		PSIZE, ATTR6                                Input point size, Generic Attribute 6
    		BLENDINDICES, ATTR7                         Generic Attribute 7
    		TEXCOORD0-TEXCOORD7, ATTR8-ATTR15           Input texture coordinates(texcoord0-texcoord7), Generic Attributes 8–15
    		TANGENT, ATTR14                             Generic Attribute 14
    		BINORMAL, ATTR15                            Generic Attribute 15

    Table 24 summarizes the valid binding semantics for varying output parameters
    in the vp30 profile. These binding semantics map to NV_vertex_program2 output registers. The
    two sets act as aliases to each other.
    	

    Table 24 vp30 Varying Output Binding Semantics
    		Binding Semantics                           Name Corresponding Data
    		POSITION, HPOS                              Output position
    		PSIZE, PSIZ                                 Output point size
    		FOG, FOGC                                   Output fog coordinate
    		COLOR0, COL0                                Output primary color
    		COLOR1, COL1                                Output secondary color
    		BCOL0                                       Output backface primary color
    		BCOL1                                       Output backface secondary color
    		TEXCOORD0-TEXCOORD7, TEX0-TEX7              Output texture coordinates
    		CLP0-CL5                                    Output Clip distances


Using NVidia CG texture:
------------------------
    CGparameter BumpMapParam = cgGetNamedParameter(cgprog, "BumpTexture");
	cgGLSetTextureParameter(BumpMapParam, texture_gl_object_id);
    cgGLEnableTextureParameter(BumpMapParam);