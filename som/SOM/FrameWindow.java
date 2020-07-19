/* FrameWindow - Decompiled by JODE
 * http://www.informatik.uni-oldenburg.de/~delwi/jode/jode.html
 * Send comments or bug reports to jochen@gnu.org
 */
package SOM;

import java.awt.*;
import java.awt.event.*;
import java.util.Random;

public class FrameWindow
    extends Frame
    implements ActionListener, ItemListener, Runnable
{
    private Label typeLabel = new Label();
    private SOMCanvas canvas = new SOMCanvas();
    private Panel dataPanel = new Panel();
    private GridBagLayout gbl = new GridBagLayout();
    private GridBagConstraints gbc = new GridBagConstraints();
    private Panel checkboxPanel = new Panel();
    private Panel iteratePanel = new Panel();
    private Button initButton = new Button("Initialize");
    private Button startButton = new Button("Start");
    private Button stopButton = new Button("Stop");
    private TextField iterationTextField = new TextField("1", 10);
    private Label blockLabel = new Label("Blocks:");
    private Checkbox block1checkbox = new Checkbox("Block 1");
    private Checkbox block2checkbox = new Checkbox("Block 2");
    private Checkbox block3checkbox = new Checkbox("Block 3");
    private Label unitsLabel = new Label("Number of units:");
    private TextField numberOfUnitsTextField = new TextField("100", 6);
    private Label learningRateLabel = new Label("Starting learning rate:");
    private TextField learningRateTextField = new TextField("0.5", 6);
    private Label decayRateLabel = new Label("Decay of learning rate:");
    private TextField decayRateTextField = new TextField("0.0001", 6);
    private Label neighbourhoodLabel = new Label("Neighbourhood:");
    private Choice neighbourhoodChoice = new Choice();
    private String spaces = new String("                                  ");
    private Label actualLearningRate = new Label("Actual learning rate:" + spaces);
    private Label numberOfIterations = new Label("Number of iterations:" + spaces);
    private Label minDistance = new Label("Minimal distance:" + spaces);
    private SOMNet somnet;
    private Random random;
    private int not;
    private double mindist;

    private Color backgroundColor = Color.blue;
    private Color foregroundColor = Color.red;

    private boolean running = false;
    
    public FrameWindow(Random random_1_1_) {
	random = random_1_1_;
	add(typeLabel, "North");
	add(canvas, "Center");
	add(dataPanel, "South");
	dataPanel.setLayout(gbl);
	gbc.gridx = 0;
	gbc.gridy = 0;
	gbc.gridwidth = 2;
	gbc.gridheight = 1;
	gbc.anchor = 17;
	gbl.setConstraints(unitsLabel, gbc);
	dataPanel.add(unitsLabel);
	gbc.gridx = 2;
	gbc.gridy = 0;
	gbc.gridwidth = 1;
	gbc.gridheight = 1;
	gbl.setConstraints(numberOfUnitsTextField, gbc);
	dataPanel.add(numberOfUnitsTextField);
	gbc.gridx = 3;
	gbc.gridy = 0;
	gbc.gridwidth = 2;
	gbc.gridheight = 3;
	gbc.anchor = 17;
	gbl.setConstraints(neighbourhoodLabel, gbc);
	dataPanel.add(neighbourhoodLabel);
	gbc.gridx = 5;
	gbc.gridy = 0;
	gbc.gridwidth = 1;
	gbc.gridheight = 3;
	gbl.setConstraints(neighbourhoodChoice, gbc);
	dataPanel.add(neighbourhoodChoice);
	gbc.gridx = 6;
	gbc.gridy = 0;
	gbc.gridwidth = 1;
	gbc.gridheight = 3;
	gbc.anchor = 17;
	gbl.setConstraints(blockLabel, gbc);
	dataPanel.add(blockLabel);
	gbc.gridx = 7;
	gbc.gridy = 0;
	gbc.gridwidth = 0;
	gbc.gridheight = 3;
	gbl.setConstraints(checkboxPanel, gbc);
	dataPanel.add(checkboxPanel);
	gbc.gridx = 0;
	gbc.gridy = 1;
	gbc.gridwidth = 2;
	gbc.gridheight = 1;
	gbc.anchor = 17;
	gbl.setConstraints(learningRateLabel, gbc);
	dataPanel.add(learningRateLabel);
	gbc.gridx = 2;
	gbc.gridy = 1;
	gbc.gridwidth = 1;
	gbc.gridheight = 1;
	gbl.setConstraints(learningRateTextField, gbc);
	dataPanel.add(learningRateTextField);
	gbc.gridx = 0;
	gbc.gridy = 2;
	gbc.gridwidth = 2;
	gbc.gridheight = 1;
	gbc.anchor = 17;
	gbl.setConstraints(decayRateLabel, gbc);
	dataPanel.add(decayRateLabel);
	gbc.gridx = 2;
	gbc.gridy = 2;
	gbc.gridwidth = 1;
	gbc.gridheight = 1;
	gbl.setConstraints(decayRateTextField, gbc);
	dataPanel.add(decayRateTextField);
	gbc.gridx = 2;
	gbc.gridy = 3;
	gbc.gridwidth = 1;
	gbc.gridheight = 1;
	gbl.setConstraints(initButton, gbc);
	dataPanel.add(initButton);
	gbc.gridx = 3;
	gbc.gridy = 3;
	gbc.gridwidth = 1;
	gbc.gridheight = 1;
	gbl.setConstraints(startButton, gbc);
	dataPanel.add(startButton);
	gbc.gridx = 4;
	gbc.gridy = 3;
	gbc.gridwidth = 1;
	gbc.gridheight = 1;
	gbl.setConstraints(stopButton, gbc);
	dataPanel.add(stopButton);
	gbc.gridx = 3;
	gbc.gridy = 4;
	gbc.gridwidth = 1;
	gbc.gridheight = 1;
	gbl.setConstraints(iterationTextField, gbc);
	dataPanel.add(iterationTextField);
	gbc.gridx = 0;
	gbc.gridy = 6;
	gbc.gridwidth = 4;
	gbc.gridheight = 1;
	gbc.anchor = 17;
	gbl.setConstraints(actualLearningRate, gbc);
	dataPanel.add(actualLearningRate);
	gbc.gridx = 0;
	gbc.gridy = 7;
	gbc.gridwidth = 4;
	gbc.gridheight = 1;
	gbc.anchor = 17;
	gbl.setConstraints(numberOfIterations, gbc);
	dataPanel.add(numberOfIterations);
	gbc.gridx = 0;
	gbc.gridy = 8;
	gbc.gridwidth = 4;
	gbc.gridheight = 1;
	gbc.anchor = 17;
	gbl.setConstraints(minDistance, gbc);
	dataPanel.add(minDistance);
	neighbourhoodChoice.add(Integer.toString(0));
	neighbourhoodChoice.add(Integer.toString(4));
	neighbourhoodChoice.add(Integer.toString(8));
	neighbourhoodChoice.select(1);
	checkboxPanel.setLayout(new BorderLayout());
	checkboxPanel.add(block1checkbox, "North");
	checkboxPanel.add(block2checkbox, "Center");
	checkboxPanel.add(block3checkbox, "South");
	initButton.addActionListener(this);
	startButton.addActionListener(this);
	stopButton.addActionListener(this);
	block1checkbox.addItemListener(this);
	block2checkbox.addItemListener(this);
	block3checkbox.addItemListener(this);
	setSize(500, 450);
	validate();
    }
    
    public FrameWindow(Random random_1_3_, String string_2_4_) {
	this(random_1_3_);
	typeLabel.setText(string_2_4_);
    }


    public void update(Graphics g) {
	if( somnet != null && canvas != null ) {
	    somnet.drawNet(canvas.getGraphics(), canvas.getSize(),backgroundColor);
	}
    }

    public void init() {
	canvas.getGraphics().setColor(foregroundColor);
	canvas.setBackground(backgroundColor);
	canvas.getGraphics().clearRect(0, 0, canvas.getSize().width, canvas.getSize().height);
	actualLearningRate.setText("Actual learning rate:");
	numberOfIterations.setText("Number of iterations:");
	minDistance.setText("Minimal distance:");
	not = 0;
	try {
	    somnet = new SOMNet(Integer.parseInt(numberOfUnitsTextField.getText()), 
				Double.valueOf(learningRateTextField.getText()).doubleValue(), 
				Double.valueOf(decayRateTextField.getText()).doubleValue(), 
				2, 
				Integer.parseInt(neighbourhoodChoice.getSelectedItem()));
	} catch (NumberFormatException numberformatexception_2_7_) {
	    System.out.println("Illegal number!" + numberformatexception_2_7_);
	} catch (BadInitializerException badinitializerexception_2_8_) {
	    System.out.println("Illegal network size!" + badinitializerexception_2_8_);
	}

	// setting the blocks state
	if (block1checkbox.getState()) {
	    somnet.setBlock(0);
	} else {
	    somnet.deleteBlock(0);
        }
	if (block2checkbox.getState()) {
	    somnet.setBlock(1);
	} else {
	    somnet.deleteBlock(1);
        }
	if (block3checkbox.getState()) {
	    somnet.setBlock(2);
	} else {
	    somnet.deleteBlock(2);
        }
	canvas.setNet(somnet);
	somnet.drawNet(canvas.getGraphics(), canvas.getSize(),backgroundColor);
    }

    public void iterate() {
	canvas.getGraphics().setColor(Color.red);
	canvas.setBackground(Color.blue);
	int i_2_9_ = Integer.parseInt(iterationTextField.getText());
	double[] ds_3_10_ = new double[2];
	Input input_4_11_ = new Input();
	for (int i_6_12_ = 0; i_6_12_ < i_2_9_; i_6_12_++) {
	    for (boolean bool_5_13_ = false; !bool_5_13_; bool_5_13_ = somnet.validInput(input_4_11_)) {
		ds_3_10_[0] = 2.0 * random.nextDouble() - 1.0;
		ds_3_10_[1] = 2.0 * random.nextDouble() - 1.0;
		input_4_11_.setFeatures(ds_3_10_, 2);
	    }
	    try {
		mindist = somnet.iterateNet(input_4_11_);
	    } catch (TypeMismatchException typemismatchexception_14_) {
		System.out.println("Bad input dimensions!");
	    }
	}
	not += i_2_9_;
	//canvas.setVisible(false);
	//canvas.getGraphics().clearRect(0, 0, canvas.getSize().width, canvas.getSize().height);
	somnet.drawNet(canvas.getGraphics(), canvas.getSize(), backgroundColor);
	//canvas.setVisible(true);
	actualLearningRate.setText("Actual learning rate:" + somnet.getLearningRate());
	numberOfIterations.setText("Number of iterations:" + not);
	minDistance.setText("Minimal distance:" + mindist);
    }
    
    public void actionPerformed(ActionEvent actionevent_1_6_) {
	if (actionevent_1_6_.getSource() == initButton) {
	    init();
	} else if (actionevent_1_6_.getSource() == startButton) {
	    running = true;
	} else if (actionevent_1_6_.getSource() == stopButton) {
	    running = false;
	}
    }

    public void itemStateChanged(ItemEvent itemevent_1_6_) {
	// changing the blocks'states
	if (itemevent_1_6_.getSource() == block1checkbox) {
	    if (block1checkbox.getState()) {
		somnet.setBlock(0);
	    } else {
		somnet.deleteBlock(0);
	    }
	    repaint();
	} else if (itemevent_1_6_.getSource() == block2checkbox) {
	    if (block2checkbox.getState()) {
		somnet.setBlock(1);
	    } else {
		somnet.deleteBlock(1);
	    }
	    repaint();
	} else if (itemevent_1_6_.getSource() == block3checkbox) {
	    if (block3checkbox.getState()) {
		somnet.setBlock(2);
	    } else {
		somnet.deleteBlock(2);
	    }
	    repaint();
	}
    }

    public void run() {
	while( true ) {
	    if( running ) {
		iterate();
	    }
	}
    }
}



















