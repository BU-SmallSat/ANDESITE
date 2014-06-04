package xbee;

public class XbeeInfo {
	private int sh;
	private int sl;
	private int my;
	/*private String ni;
	private int defaultMy = 0xFFFE;
	private int shBroadcast = 0x0;
	private int slBroadcast = 0x0000FFFF;*/
	
	// Constuctor argument takes 64-bit address, 16-bit address, and node identifier
	public XbeeInfo(int sh, int sl, int my) {
		this.sh = sh;
		this.sl = sl;
		this.my = my;
		//this.ni = ni;
	}
	
	/*public XbeeInfo()
	{
		this.sh = shBroadcast;
		this.sl = slBroadcast;
		this.my = defaultMy;
		this.ni = "BROADCAST";
	}*/
	
	/*public XbeeInfo(String sh, String sl, String my, String ni)
	{
		this.sh = Integer.parseInt(sh, 16);
		this.sl = Integer.parseInt(sl, 16);
		this.my = Integer.parseInt(my, 16);
		this.ni = ni;
	}*/
	
	/*//If we don't know the MY
	public XbeeInfo(int sh, int sl, String ni) {
		this.sh = sh;
		this.sl = sl;
		this.ni = ni;
		this.my = defaultMy;
	}*/
	
	//If we don't know the NI
	/*public XbeeInfo(int sh, int sl, int my)
	{
		this.sh = sh;
		this.sl = sl;
		this.my = my;
	}*/
	
	//If we don't know the NI or the MY
	/*public XbeeInfo(int sh, int sl)
	{
		this.sh = sh;
		this.sl = sl;
		this.my = defaultMy;
	}*/
	
	public void setMy(int my) {
		this.my = my;
	}
	
	/*public String getNi() {
		return this.ni;
	}
	
	public void setNi(String ni) {
		this.ni = ni;
	}*/
	
	public int getSh() {
		return this.sh;
	}
	
	public int getSl() {
		return this.sl;
	}
	
	public int getMy() {
		return this.my;
	}
}
