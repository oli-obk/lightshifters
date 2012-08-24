#ifndef PAGE_H
#define PAGE_H

#include <Gosu/Window.hpp>

class Page {

private:
	Page(const Page& rhs);
	Page& operator=(const Page& rhs);
	
protected:
	Page();
	
public:
	virtual ~Page();
	virtual void buttonDown(Gosu::Button);
	virtual void buttonUp(Gosu::Button);
	virtual void draw();
	virtual void loseFocus();
	virtual bool needsCursor() const;
	virtual bool needsRedraw() const;
	virtual void releaseMemory();
	virtual void touchBegan(Gosu::Touch touch);
	virtual void touchEnded(Gosu::Touch touch);
	virtual void touchMoved(Gosu::Touch touch);
	virtual void update();
};

#endif // PAGE_H
