import web
import sys
import json

render = web.template.render('templates/')
urls = ( r'/api/newdeveloper/lights/1(.*)', 'LightIndex1',
         r'/api/newdeveloper/lights', 'LightIndex2',
         r'/api/zmc/lights', 'LightIndex3',
         r'/api/zmc2/lights', 'LightIndex4',
)


ret_status = """{
    "state": {
        "hue": 50000,
        "on": true,
        "effect": "none",
        "alert": "none",
        "bri": 200,
        "sat": 200,
        "ct": 500,
        "xy": [0.5, 0.5],
        "reachable": true,
        "colormode": "hs"
    },
    "type": "Living Colors",
    "name": "LC 1",
    "modelid": "LC0015",
    "swversion": "1.0.3",     
    "pointsymbol": {
        "1": "none",
        "2": "none",
        "3": "none",
        "4": "none",
        "5": "none",
        "6": "none",
        "7": "none",
        "8": "none"
    }
}"""
ret_status = json.loads(ret_status)


class LightIndex1:
   def GET(self, name=None):
       return ret_status


   def POST(self, name=None):
       print "Post ......"
       data = web.data()
       print data 
       return data

   def PUT(self, name=None):
       print "Put ......"
       data= web.data()
       print data
       r_data = json.loads(data)
       ret_status["state"]["on"] = r_data["on"]
       return data
   
   def DELETE(self, name=None):
       print "DELETE ......"
       return render.index(name)

class LightIndex2:
   def GET(self, name=None):
      lights = """{
	"1": {
		"name": "Bedroom"
	},
	"2": {
		"name": "Kitchen"
	}
      }"""
      lights = json.loads(lights)
      print type(lights)
      return lights

class LightIndex3:
   def GET(self, name=None):
      lights = """{
	"3": {
		"name": "Whitehouse"
	},
	"4": {
		"name": "Desktop"
	}
      }"""
      lights = json.loads(lights)
      print type(lights)
      return lights


class LightIndex4:
   def GET(self, name=None):
      lights = """{
	"5": {
		"name": "test1"
	},
	"6": {
		"name": "test2"
	}
      }"""
      lights = json.loads(lights)
      print type(lights)
      return lights

 
if __name__ =="__main__":
    sys.argv.append("80")
    app = web.application(urls, globals())
    app.run()
