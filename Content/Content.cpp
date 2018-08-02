#include "Content.h"

Content::Content()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);

	ofSetWindowTitle("Digital Painting");

	m_whiteThresh = 220;
	m_snapshot = false;
	m_showGui = true;

	m_numPoints = 1000;
	m_numRays = 10000;

	m_shader.load("vert.glsl", "frag.glsl");
	m_compute.load( "compute.glsl");

	m_plane.set(ofGetWidth(), ofGetHeight(), 10, 10);
	m_outputTexture.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA32F);

	m_computeReadPixels.allocate(m_outputTexture.getWidth(), m_outputTexture.getHeight(), 4);//use same specs as outputTex
	m_computeReadBuffer.allocate(m_computeReadPixels, GL_DYNAMIC_READ);


	// GENERATE POINTS FROM IMAGE
	// load an image from disk
	m_image.load("paint1.png");
	// we're going to load a ton of points into an ofMesh
	m_mesh.setMode(OF_PRIMITIVE_POINTS);
	// loop through the image in the x and y axes
	for(int i = 0; i<m_numPoints; i++)
	{
		int x = ofRandom(float(m_image.getWidth()));
		int y = ofRandom(float(m_image.getHeight()));

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
	m_pointsBuffer.allocate(m_points, GL_DYNAMIC_DRAW);
	m_pointsBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 1);
	setRays();


	// SETUP RAY BUFFER ON GPU
	m_cam.setVFlip(true); //flip for upside down image
	m_cam.setFarClip(100000000.);


	m_plane.set(ofGetWidth(), ofGetHeight(), 10, 10);
	m_plane.mapTexCoords(0, 0, ofGetWidth(), ofGetHeight());
	ofEnableDepthTest();
	glPointSize(6);
	ofSetBackgroundColor(50, 50, 50);

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
	for(int i = 0; i<m_numRays; i++)
	{
		int x = ofRandom(ofGetWidth());
		int y = ofRandom(ofGetHeight());
		float px = (2. * ((x + .5) / ofGetWidth()) - 1.) * tanTheta* ar;
		float py = (1. - 2. * ((y + .5) / ofGetHeight())) * tanTheta;
		float pz = (ofGetHeight() / 2.) / tanTheta;
		glm::vec3 pos(px, py, -pz);
		auto rayWorldPos = m_cam.cameraToWorld(pos);
		auto rayWorldDir = rayWorldPos - rayWorldOrigin;
		rayWorldDir = glm::normalize(rayWorldDir);

		m_rays[i].m_id = glm::ivec2(x, y);
		m_rays[i].m_origin = ofVec4f(rayWorldOrigin.x, rayWorldOrigin.y, rayWorldOrigin.z, 1.);
		m_rays[i].m_dir = rayWorldDir;
		i++;
	}

	// buffer and vbo allocation 
	m_rayBuffer.allocate(m_rays, GL_DYNAMIC_DRAW);
	m_rayBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 0);

	m_compute.getShader().begin();
	m_outputTexture.bindAsImage(0, GL_READ_WRITE);
	m_compute.getShader().setUniform1i("u_numPoints", m_numPoints);
	m_compute.getShader().dispatchCompute(100, 100, 1);
	m_compute.getShader().end();
}

void Content::readComputeOutput()
{
	m_outputTexture.copyTo(m_computeReadBuffer);
	float* p = m_computeReadBuffer.map<float>(GL_READ_ONLY);
	m_computeReadPixels.setFromPixels(p, m_outputTexture.getWidth(), m_outputTexture.getHeight(), 4);
	m_computeReadBuffer.unmap();
	m_computePixelCache.clear();
	m_computePixelCache.resize(0);
	for (int i = 0; i < m_computeReadPixels.size(); i+= 4)
	{
			m_computePixelCache.push_back(glm::vec4(
				m_computeReadPixels[i], 
				m_computeReadPixels[i + 1], 
				m_computeReadPixels[i + 2], 
				m_computeReadPixels[i + 3]));
	}

}

void Content::update()
{
	m_shader.update();
	m_compute.update();
}

void Content::draw()
{
	///// WORLD
	{
		m_cam.begin();		ofScale(2, -2, 2); // flip the y axis and zoom in a bit
		ofTranslate(-m_image.getWidth() / 2, -m_image.getHeight() / 2);		ofPointSmooth();		m_mesh.draw();
		m_cam.end();		m_cam.begin();
		m_outputTexture.bind();
		m_shader.getShader().begin();
		ofTranslate(2*ofGetWidth(), 0);
		ofSetColor(255, 255, 255);
		m_plane.draw();
		m_shader.getShader().end();
		m_outputTexture.unbind();
		m_cam.end();


		//m_outputTexture.draw(0, 0);


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
			m_cam.begin();
			ofDrawGrid(5000, 5, true, true, true, true);
			drawRayDirs();
			m_cam.end();
		}

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
	if (!m_rays.empty())
	{
		glm::vec3 o(m_rays[0].m_origin.x, m_rays[0].m_origin.y, m_rays[0].m_origin.z);
		int skip = 1000;
		for (int i = 0; i < m_rays.size(); )
		{
			ofSetColor(255, 0, 0, 255);
			ofDrawLine(o, o + m_rays[i].m_dir * 10000);
			i += skip;
		}
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
		for(int i = 0; i< 100;i ++)
			setRays();
		readComputeOutput();;
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
