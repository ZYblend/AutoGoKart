import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Joy
from std_msgs.msg import String
from geometry_msgs.msg import PointStamped

class Command_Node(Node):
	def __init__(self):
		super().__init__("gokart_command")
		self.get_logger().info("Node Created")
		self.subscribe = self.create_subscription(Joy,"/control_command",self.getControll,10)
		self.sbw_publisher = self.create_publisher(PointStamped,"/sbw_control",10)
		
	def map(self,input):
		value = float(input +1) / 2
		return -30 + (value*60)
		
	def getControll(self,msg):
		data = msg
		msg = PointStamped();
		msg.header.frame_id = "sbw_input"
		msg.point.x = self.map(data.point.x)
		msg.point.y = self.rl
		msg.point.z = 0.0
		self.publisher.publish(msg)
	
def main(args=None):
	rclpy.init(args=args)
	node = Command_Node()
	node.get_logger().info("Object created")
	rclpy.spin(node)
	rclpy.shutdown()
	
if __name__ == "__main__":
	main() 
