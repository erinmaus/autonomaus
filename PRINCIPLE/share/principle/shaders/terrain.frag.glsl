#version 140

uniform sampler2D principle_Texture;

const vec4 uAtlasMeta = vec4(128.0, 16.0, 1 / 2048.0, 6.0);

in vec3 frag_Position;
in vec4 frag_TextureSlot;
in vec4 frag_TextureSize;
in vec4 frag_TextureScale;
in vec4 frag_TextureWeight;
in vec4 frag_Color;

out vec4 out_Color;

vec2 compute_texture_coordinates(vec4 v)
{
	vec4 d = vec4(128.0, 16.0, 1.0 / 2016.0, 6.0);
	float m = v.p;
	float o = v.q;
	float p = d.s;
	float l = d.t;
	float u = d.p;
	m = floor(m + 0.5f);
	float q = floor(m / p);
	float L = m - p * q;
	float a = v.s;
	float C = v.t;
	float N = 0.5f / o;

	const vec2 i = vec2(1.0f);

	vec2 c;
	// LOOKUP_MODE_REPEAT
	//c = mod(vec2(a, C), i);
	// LOOKUP_MODE_CLAMP
	c = clamp(vec2(a, C), vec2(N), i - vec2(N));

	c = vec2(L, q) * l + c * o;
	c *= u;

	return c;
}


void getTexelBias(vec4 v,vec4 d,vec2 t,vec2 g,float S,sampler2D f,sampler2D T,out vec4 s,out vec4 P) {
    float h=v.p,o=v.q;
    float p=d.s,l=d.t,m=d.p,D=d.q;
    h=floor(h+.5);
    float q=floor(h/p),L=h-p*q;
    float i=v.s,a=v.t,N=.5/o;
    vec2 c;
    const vec2 u=vec2(1.);
    c=mod(vec2(i,a),u);
    c=vec2(L,q)*l+c*o;
    c*=m;
    s=texture2D(f,c);
}

void getTexelSRGB(vec4 v,vec4 d,vec2 f,vec2 s,float c,sampler2D o,sampler2D g,out vec4 t,out vec4 P) {
    getTexelBias(v,d,f,s,c,o,g,t,P);
}

void SampleTerrainTextures(out vec4 t,out float v,inout vec3 u,vec3 d,vec4 y,vec4 p) {
    vec4 g,T,q;
    vec3 S=frag_TextureScale.stp,e=8./(S*512.0);
    vec2 m=d.sp*e[0],s=d.sp*e[1],G=d.sp*e[2];
    g.st=m;
    T.st=s;
    q.st=G;
    g.pq=vec2(frag_TextureSlot[0],frag_TextureSize[0]);
    T.pq=vec2(frag_TextureSlot[1],frag_TextureSize[1]);
    q.pq=vec2(frag_TextureSlot[2],frag_TextureSize[2]);

    vec3 r=vec3(0.),n=vec3(0.);
    vec2 a=r.sp*e[0],i=n.sp*e[0],o=r.sp*e[1],f=n.sp*e[1],h=r.sp*e[2],c=n.sp*e[2];
    vec3 Y=mod(p.stp,vec3(16.))-vec3(.25),l=(p.stp-vec3(.25)-Y)/vec3(255.);
    Y*=vec3(1./15.);
    vec4 x=vec4(vec3(.580392),y.q),H=mix(y,x,l.s)*(1.+Y.s),U=mix(y,x,l.t)*(1.+Y.t),D=mix(y,x,l.p)*(1.+Y.p);
    vec4 N,P,E,O=vec4(0.,.5,1.,.5),A=vec4(0.,.5,1.,.5),R=vec4(0.,.5,1.,.5);
    getTexelSRGB(g,uAtlasMeta,a,i,0.,principle_Texture,principle_Texture,N,O);
    getTexelSRGB(T,uAtlasMeta,o,f,0.,principle_Texture,principle_Texture,P,A);
    getTexelSRGB(q,uAtlasMeta,h,c,0.,principle_Texture,principle_Texture,E,R);
    N*=H;
    P*=U;
    E*=D;

    N=mix(H,sqrt(N),l.s);
    P=mix(U,sqrt(P),l.t);
    E=mix(D,sqrt(E),l.p);
    t=N*frag_TextureWeight.s+P*frag_TextureWeight.t+E*frag_TextureWeight.p;
    t.q=1.;
    vec3 X=step(vec3(.5),fract(p.q*vec3(.25,.125,.0625))),I=vec3(N.q,P.q,E.q)*frag_TextureWeight.stp,F=I*X;
    v=F.s+F.t+F.p;
}

vec4 sample_terrain()
{
    vec3 d=frag_Position.stp;
    vec3 v=vec3(0.0);
    vec4 u=vec4(0.,0.,0.,1.);
    float p=0.;
    SampleTerrainTextures(u,p,v,d,vec4(1.0),vec4(256.0));
    return u;
}


void main()
{
	vec4 sample = sample_terrain();

	out_Color = frag_Color * (vec4(sample.rgb, 1) + vec4(0.3, 0.3, 0.3, 0.0));
}
