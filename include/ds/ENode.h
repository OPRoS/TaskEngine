#pragma once

class ENode
{

private:
	int		m_type;
	char*	m_value;
	

public:
	ENode(void);
	virtual ~ENode(void);

	void setValue(char*);
	char* getValue();
};
