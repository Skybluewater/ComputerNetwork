import java.awt.BorderLayout;
import java.awt.Button;
import java.awt.Color;
import java.awt.Font;
import java.awt.GridLayout;
import java.awt.Image;
import java.awt.TextArea;
import java.awt.TextField;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;
import java.util.TooManyListenersException;

import gnu.io.CommPortIdentifier;
import gnu.io.NoSuchPortException;
import gnu.io.PortInUseException;
import gnu.io.SerialPort;
import gnu.io.SerialPortEvent;
import gnu.io.SerialPortEventListener;
import gnu.io.UnsupportedCommOperationException;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.SwingConstants;
import javax.swing.border.EmptyBorder;

public class RS232 extends JFrame implements ActionListener, SerialPortEventListener {

	/**
	 * JDK Serial Version UID
	 */
	private static final long serialVersionUID = -7270865686330790103L;

	protected int WIN_WIDTH = 380;

	protected int WIN_HEIGHT = 300;

	private JComboBox<?> portCombox, rateCombox, dataCombox, stopCombox, parityCombox;

	private Button openPortBtn, closePortBtn, sendMsgBtn;

	private TextField sendTf;

	private TextArea readTa;

	private JLabel statusLb;

	private String portname, rate, data, stop, parity;

	protected CommPortIdentifier portId;

	protected Enumeration<?> ports;

	protected List<String> portList;

	protected SerialPort serialPort;

	protected OutputStream outputStream = null;

	protected InputStream inputStream = null;

	protected String mesg;

	protected int sendCount, reciveCount;

	/**
	 * 默认构造函数
	 */
	public RS232() {
		super("RS232串口通信测试程序");
		setSize(WIN_WIDTH, WIN_HEIGHT);
		setLocationRelativeTo(null);
		Image icon = null;
		setIconImage(icon);
		setResizable(false);
		scanPorts();
		initComponents();
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setVisible(true);
	}

	/**
	 * 初始化各UI组件
	 * 
	 */
	public void initComponents() {
		// 共用常量
		Font lbFont = new Font("微软雅黑", Font.TRUETYPE_FONT, 14);

		// 创建左边面板
		JPanel northPane = new JPanel();
		northPane.setLayout(new GridLayout(1, 1));
		// 设置左边面板各组件
		JPanel leftPane = new JPanel();
		leftPane.setOpaque(false);
		leftPane.setLayout(new GridLayout(3, 2));
		JLabel portnameLb = new JLabel("串口号：");
		portnameLb.setFont(lbFont);
		portnameLb.setHorizontalAlignment(SwingConstants.RIGHT);
		portCombox = new JComboBox<String>((String[]) portList.toArray(new String[0]));
		portCombox.addActionListener(this);
		JLabel databitsLb = new JLabel("数据位：");
		databitsLb.setFont(lbFont);
		databitsLb.setHorizontalAlignment(SwingConstants.RIGHT);
		dataCombox = new JComboBox<Integer>(new Integer[] { 5, 6, 7, 8 });
		dataCombox.setSelectedIndex(3);
		dataCombox.addActionListener(this);
		JLabel parityLb = new JLabel("校验位：");
		parityLb.setFont(lbFont);
		parityLb.setHorizontalAlignment(SwingConstants.RIGHT);
		parityCombox = new JComboBox<String>(new String[] { "NONE", "ODD", "EVEN", "MARK", "SPACE" });
		parityCombox.addActionListener(this);
		// 添加组件至面板
		leftPane.add(portnameLb);
		leftPane.add(portCombox);
		leftPane.add(databitsLb);
		leftPane.add(dataCombox);
		leftPane.add(parityLb);
		leftPane.add(parityCombox);

		// 创建右边面板
		JPanel rightPane = new JPanel();
		rightPane.setLayout(new GridLayout(3, 2));
		// 设置右边面板各组件
		JLabel baudrateLb = new JLabel("波特率：");
		baudrateLb.setFont(lbFont);
		baudrateLb.setHorizontalAlignment(SwingConstants.RIGHT);
		rateCombox = new JComboBox<Integer>(new Integer[] { 2400, 4800, 9600, 14400, 19200, 38400, 56000 });
		rateCombox.setSelectedIndex(2);
		rateCombox.addActionListener(this);
		JLabel stopbitsLb = new JLabel("停止位：");
		stopbitsLb.setFont(lbFont);
		stopbitsLb.setHorizontalAlignment(SwingConstants.RIGHT);
		stopCombox = new JComboBox<String>(new String[] { "1", "2", "1.5" });
		stopCombox.addActionListener(this);
		openPortBtn = new Button("打开端口");
		openPortBtn.addActionListener(this);
		closePortBtn = new Button("关闭端口");
		closePortBtn.addActionListener(this);
		// 添加组件至面板
		rightPane.add(baudrateLb);
		rightPane.add(rateCombox);
		rightPane.add(stopbitsLb);
		rightPane.add(stopCombox);
		rightPane.add(openPortBtn);
		rightPane.add(closePortBtn);
		// 将左右面板组合添加到北边的面板
		northPane.add(leftPane);
		northPane.add(rightPane);

		// 创建中间面板
		JPanel centerPane = new JPanel();
		// 设置中间面板各组件
		sendTf = new TextField(42);
		readTa = new TextArea(8, 50);
		readTa.setEditable(false);
		readTa.setBackground(new Color(225, 242, 250));
		centerPane.add(sendTf);
		sendMsgBtn = new Button(" 发送 ");
		sendMsgBtn.addActionListener(this);
		// 添加组件至面板
		centerPane.add(sendTf);
		centerPane.add(sendMsgBtn);
		centerPane.add(readTa);

		// 设置南边组件
		statusLb = new JLabel();
		statusLb.setText(initStatus());
		statusLb.setOpaque(true);

		// 获取主窗体的容器,并将以上三面板以北、中、南的布局整合
		JPanel contentPane = (JPanel) getContentPane();
		contentPane.setLayout(new BorderLayout());
		contentPane.setBorder(new EmptyBorder(0, 0, 0, 0));
		contentPane.setOpaque(false);
		contentPane.add(northPane, BorderLayout.NORTH);
		contentPane.add(centerPane, BorderLayout.CENTER);
		contentPane.add(statusLb, BorderLayout.SOUTH);
	}

	/**
	 * 初始化状态标签显示文本
	 * 
	 * @return String
	 */
	public String initStatus() {
		portname = portCombox.getSelectedItem().toString();
		rate = rateCombox.getSelectedItem().toString();
		data = dataCombox.getSelectedItem().toString();
		stop = stopCombox.getSelectedItem().toString();
		parity = parityCombox.getSelectedItem().toString();

		StringBuffer str = new StringBuffer("当前串口号:");
		str.append(portname).append(" 波特率:");
		str.append(rate).append(" 数据位:");
		str.append(data).append(" 停止位:");
		str.append(stop).append(" 校验位:");
		str.append(parity);
		return str.toString();
	}

	/**
	 * 扫描本机的所有COM端口
	 * 
	 */
	public void scanPorts() {
		portList = new ArrayList<String>();
		Enumeration<?> en = CommPortIdentifier.getPortIdentifiers();
		CommPortIdentifier portId;
		while (en.hasMoreElements()) {
			portId = (CommPortIdentifier) en.nextElement();
			if (portId.getPortType() == CommPortIdentifier.PORT_SERIAL) {
				String name = portId.getName();
				if (!portList.contains(name)) {
					portList.add(name);
				}
			}
		}
		if (null == portList || portList.isEmpty()) {
			showErrMesgbox("未找到可用的串行端口号,程序无法启动!");
			System.exit(0);
		}
	}

	/**
	 * 打开串行端口
	 * 
	 */
	public void openSerialPort() {
		// 获取要打开的端口
		try {
			portId = CommPortIdentifier.getPortIdentifier(portname);
		} catch (NoSuchPortException e) {
			showErrMesgbox("抱歉,没有找到" + portname + "串行端口号!");
			setComponentsEnabled(true);
			return;
		}
		// 打开端口
		try {
			serialPort = (SerialPort) portId.open("RS232", 2000);
			statusLb.setText(portname + "串口已经打开!");
		} catch (PortInUseException e) {
			showErrMesgbox(portname + "端口已被占用,请检查!");
			setComponentsEnabled(true);
			return;
		}

		// 设置端口参数
		try {
			int rate = Integer.parseInt(this.rate);
			int data = Integer.parseInt(this.data);
			int stop = stopCombox.getSelectedIndex() + 1;
			int parity = parityCombox.getSelectedIndex();
			serialPort.setSerialPortParams(rate, data, stop, parity);
		} catch (UnsupportedCommOperationException e) {
			showErrMesgbox(e.getMessage());
		}

		// 打开端口的IO流管道
		try {
			outputStream = serialPort.getOutputStream();
			inputStream = serialPort.getInputStream();
		} catch (IOException e) {
			showErrMesgbox(e.getMessage());
		}

		// 给端口添加监听器
		try {
			serialPort.addEventListener(this);
		} catch (TooManyListenersException e) {
			showErrMesgbox(e.getMessage());
		}

		serialPort.notifyOnDataAvailable(true);
	}

	/**
	 * 给串行端口发送数据
	 * 
	 */
	public void sendDataToSeriaPort() {
		try {
			sendCount++;
			outputStream.write(mesg.getBytes());
			outputStream.flush();

		} catch (IOException e) {
			showErrMesgbox(e.getMessage());
		}

		statusLb.setText("  发送: " + sendCount + "                                      接收: " + reciveCount);
	}

	/**
	 * 关闭串行端口
	 * 
	 */
	public void closeSerialPort() {
		try {
			if (outputStream != null)
				outputStream.close();
			if (serialPort != null)
				serialPort.close();
			serialPort = null;
			statusLb.setText(portname + "串口已经关闭!");
			sendCount = 0;
			reciveCount = 0;
			sendTf.setText("");
			readTa.setText("");
		} catch (Exception e) {
			showErrMesgbox(e.getMessage());
		}
	}

	/**
	 * 显示错误或警告信息
	 * 
	 * @param msg 信息
	 */
	public void showErrMesgbox(String msg) {
		JOptionPane.showMessageDialog(this, msg);
	}

	/**
	 * 各组件行为事件监听
	 */
	public void actionPerformed(ActionEvent e) {
		if (e.getSource() == portCombox || e.getSource() == rateCombox || e.getSource() == dataCombox
				|| e.getSource() == stopCombox || e.getSource() == parityCombox) {
			statusLb.setText(initStatus());
		}
		if (e.getSource() == openPortBtn) {
			setComponentsEnabled(false);
			openSerialPort();
		}
		if (e.getSource() == closePortBtn) {
			if (serialPort != null) {
				closeSerialPort();
			}
			setComponentsEnabled(true);
		}

		if (e.getSource() == sendMsgBtn) {
			if (serialPort == null) {
				showErrMesgbox("请先打开串行端口!");
				return;
			}
			mesg = sendTf.getText();
			if (null == mesg || mesg.isEmpty()) {
				showErrMesgbox("请输入你要发送的内容!");
				return;
			}
			sendDataToSeriaPort();
		}
	}

	/**
	 * 端口事件监听
	 */
	public void serialEvent(SerialPortEvent event) {
		switch (event.getEventType()) {
		case SerialPortEvent.BI:
		case SerialPortEvent.OE:
		case SerialPortEvent.FE:
		case SerialPortEvent.PE:
		case SerialPortEvent.CD:
		case SerialPortEvent.CTS:
		case SerialPortEvent.DSR:
		case SerialPortEvent.RI:
		case SerialPortEvent.OUTPUT_BUFFER_EMPTY:
			break;
		case SerialPortEvent.DATA_AVAILABLE:
			byte[] readBuffer = new byte[50];

			try {
				while (inputStream.available() > 0) {
					inputStream.read(readBuffer);
				}
				StringBuilder receivedMsg = new StringBuilder("/-- ");
				receivedMsg.append(new String(readBuffer).trim()).append(" --/\n");
				readTa.append(receivedMsg.toString());
				reciveCount++;
				statusLb.setText("  发送: " + sendCount + "                                      接收: " + reciveCount);
			} catch (IOException e) {
				showErrMesgbox(e.getMessage());
			}
		}
	}

	/**
	 * 设置各组件的开关状态
	 * 
	 * @param enabled 状态
	 */
	public void setComponentsEnabled(boolean enabled) {
		openPortBtn.setEnabled(enabled);
		openPortBtn.setEnabled(enabled);
		portCombox.setEnabled(enabled);
		rateCombox.setEnabled(enabled);
		dataCombox.setEnabled(enabled);
		stopCombox.setEnabled(enabled);
		parityCombox.setEnabled(enabled);
	}

	/**
	 * 运行主函数
	 * 
	 * @param args
	 */
	public static void main(String[] args) {
		new RS232();
	}
}