#include "Content.h"

Content::Content()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);

	ofSetWindowTitle("Digital Painting");

	m_whiteThresh = 220;
	m_snapshot = false;
	m_showGui = true;

	m_shader.load("vert.glsl", "frag.glsl");
	m_compute.load( "compute.glsl");


	// GENERATE POINTS FROM IMAGE
	// load an image from disk
	m_image.load("paint1.png");
	// we're going to load a ton of points into an ofMesh
	m_mesh.setMode(OF_PRIMITIVE_POINTS);
	// loop through the image in the x and y axes
	int skip = 4; // load a subset of the points
	for (int y = 0; y < m_image.getHeight(); y += skip) {
		for (int x = 0; x < m_image.getWidth(); x += skip) {
			ofColor cur = m_image.getColor(x, y);
			if (cur.a > 0) {
				// the alpha value encodes depth, let's remap it to a good depth range
				float z = ofMap(cur.getBrightness(), 0, 255, -300, 300);
				cur.a = 255;
				m_mesh.addColor(cur);
				ofVec3f pos(x, y, z);
				m_mesh.addVertex(pos);


				Point point{};
				point.m_col = cur;
				point.m_pos = pos;
				m_points.push_back(point);
			}
		}
	}

	m_pointsBuffer.allocate(m_points, GL_DYNAMIC_DRAW);
	m_pointsVbo.setVertexBuffer(m_pointsBuffer, 3, sizeof(Point)); // the id
	m_pointsVbo.setColorBuffer(m_pointsBuffer, sizeof(Point), sizeof(ofVec3f) );// dir
	m_pointsBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 1);

	// SETUP RAY BUFFER ON GPU
	m_cam.setVFlip(true); //flip for upside down image
	setRays();

	m_plane.set(ofGetWidth(), ofGetHeight(), 10, 10);
	m_plane.mapTexCoords(0, 0, ofGetWidth(), ofGetHeight());
	ofEnableDepthTest();
	glPointSize(6);
	ofSetBackgroundColor(5, 5, 5);

}

void Content::setRays()
{

	// setup
	m_rayBuffer.unbindBase(GL_SHADER_STORAGE_BUFFER, 0);
	int numRays = ofGetWidth()*ofGetHeight();
	m_rays.resize(numRays);
	int i = 0;
	auto tanTheta = glm::tan(m_cam.getFov() / 2.*glm::pi<float>() / 180.);
	auto ar = m_cam.getAspectRatio();
	glm::vec3 rayWorldOrigin;
	rayWorldOrigin.x = m_cam.getPosition().x;
	rayWorldOrigin.y = m_cam.getPosition().y;
	rayWorldOrigin.z = m_cam.getPosition().z;


	// set individual rays
	for (int x = 1; x <= ofGetWidth(); x++)
	{
		for (int y = 1; y <= ofGetHeight(); y++)
		{
			float px = (2. * ((x + .5) / ofGetWidth()) - 1.) * tanTheta* ar;
			float py = (1. - 2. * ((y + .5) / ofGetHeight())) * tanTheta;
			float pz = (ofGetHeight() / 2.) / tanTheta;
			glm::vec3 pos(px, py, -pz);
			auto rayWorldPos = m_cam.cameraToWorld(pos);
			auto rayWorldDir = rayWorldPos - rayWorldOrigin;
			rayWorldDir = glm::normalize(rayWorldDir);

			m_rays[i].m_id = ofVec4f(x, y, 0, 1);
			m_rays[i].m_col = ofFloatColor(rayWorldDir.x, rayWorldDir.y, rayWorldDir.z);
			m_rays[i].m_origin = ofVec4f(rayWorldOrigin.x, rayWorldOrigin.y, rayWorldOrigin.z, 1.);
			m_rays[i].m_dir = rayWorldDir;
			i++;
		}
	}

	// buffer and vbo allocation 
	m_rayBuffer.allocate(m_rays, GL_DYNAMIC_DRAW);
	m_raysVbo.setVertexBuffer(m_rayBuffer, 4, sizeof(Ray)); // the id
	m_raysVbo.setColorBuffer(m_rayBuffer, sizeof(Ray), sizeof(ofVec4f) );// dir
	m_raysVbo.setAttributeBuffer(4, m_rayBuffer, 4, sizeof(Ray), sizeof(ofVec4f) + sizeof(ofFloatColor));//ray origin
	m_raysVbo.setAttributeBuffer(5, m_rayBuffer, 3, sizeof(Ray), 2*sizeof(ofVec4f) + sizeof(ofFloatColor));//ray dir 
	m_rayBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 0);
}

void Content::readRays()
{

	struct Ray* rays = NULL;
	try {
		rays = (Ray*)m_rayBuffer.map(GL_MAP_READ_BIT| GL_MAP_WRITE_BIT);
	}
	catch (std::runtime_error e) {
		ofLogError() << e.what();
	}
	if (glGetError() != GL_NO_ERROR || rays == NULL) {
		ofLogError() << "Could not map the shader storage buffer for particles";
	}

	m_newRays.resize(m_rays.size());
	for (int i = 0; i < m_newRays.size(); i++)
	{
		Ray r = rays[i];
		m_newRays.push_back(r);
	}

	m_rayBuffer.unmap();

}

void Content::update()
{
	m_shader.update();
	m_compute.update();

	m_compute.getShader().begin();
	m_compute.getShader().dispatchCompute(m_rays.size(), 1, 1);
	m_compute.getShader().end();
	//readRays();
}

void Content::draw()
{

	///// WORLD
	{
		m_cam.begin();
		ofScale(2, -2, 2); // flip the y axis and zoom in a bit
		ofTranslate(-m_image.getWidth() / 2, -m_image.getHeight() / 2);		ofEnableDepthTest();
		glPointSize(6);		glEnable(GL_POINT_SMOOTH); // use circular points instead of square point		m_mesh.draw();
		ofTranslate(m_image.getWidth() / 2, m_image.getHeight() / 2);

		/// SCREEN GRAB
		if (m_snapshot == true) {
			m_screenGrab.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
			string fileName = "snapshot_" + ofGetTimestampString() + ".png";
			m_screenGrab.save(fileName);
			m_screenGrabFilename = "saved " + fileName;
			m_snapshot = false;
		}

		if (m_showGui)
		{
			ofDrawGrid(5000, 5, true, true, true, true);
			drawRayDirs();
		}

		m_cam.end();
	}


	///// GUI
	if (m_showGui)
	{

		stringstream ss;
		ss << "FPS: " << ofToString(ofGetFrameRate(), 0) << endl << endl;
		ss << "CONTROLS" << endl;
		ss << "SPACE TO RESET RAYS" <<endl << endl;
		const std::string string = ss.str();
		ofDrawBitmapStringHighlight(string, glm::vec2(20, 100));
		drawInteractionArea();
	}


}

void Content::drawRayDirs()
{

	glm::vec3 o(m_rays[0].m_origin.x, m_rays[0].m_origin.y, m_rays[0].m_origin.z);
	int skip = 1000;
	for (int i = 0; i < m_rays.size(); )
	{
		ofSetColor(255, 0, 0, 255);
		ofDrawLine(o, o + m_rays[i].m_dir*10000);
		i += skip;
	}
}

void Content::drawInteractionArea()
{
	ofRectangle vp = ofGetCurrentViewport();
	float r = std::min<float>(vp.width, vp.height) * 0.5f;
	float x = vp.width * 0.5f;
	float y = vp.height * 0.5f;

	ofPushStyle();
	ofSetLineWidth(3);
	ofSetColor(255, 255, 0);
	ofNoFill();
	glDepthMask(false);
	ofSetCircleResolution(64);

	ofDrawCircle(x, y, r);
	glDepthMask(true);
	ofPopStyle();
}


void Content::exit()
{
	m_shader.exit();
	m_compute.exit();
}


void Content::keyPressed(int key)
{
	switch (key) {
	case 'x':
		m_snapshot = true;
		break;
	case 'h':
		m_showGui = !m_showGui;
		break;
	case ' ':
		setRays();
		break;
	}
}

void Content::keyReleased(int key)
{
}

void Content::mouseMoved(int x, int y)
{
}

void Content::mouseDragged(int x, int y, int button)
{
}

void Content::mousePressed(int x, int y, int button)
{
}

void Content::mouseReleased(int x, int y, int button)
{
}

void Content::mouseEntered(int x, int y)
{
}

void Content::mouseExited(int x, int y)
{
}

void Content::windowResized(int w, int h)
{
}

void Content::dragEvent(ofDragInfo info)
{
	//if (info.files.size() > 0) {

	//	m_draggedImages.assign(info.files.size(), ofImage());
	//	for (unsigned int k = 0; k < info.files.size(); k++) {
	//		m_draggedImages[k].load(info.files[k]);
	//	}
	//	m_plane.mapTexCoordsFromTexture(m_draggedImages[0].getTexture());
	//}
}

void Content::gotMessage(ofMessage msg)
{
}

bool Content::isValid()
{
	return true;
}
