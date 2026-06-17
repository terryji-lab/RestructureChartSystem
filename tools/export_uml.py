import os
import re
import json
import urllib.request

md_path = r"C:\Users\terry\.gemini\antigravity\brain\4763478b-38ec-4276-b7c1-14f6dcbe7bad\system_architecture.md"
out_dir = r"e:\ChartSystem\UML_Diagrams"

os.makedirs(out_dir, exist_ok=True)

with open(md_path, 'r', encoding='utf-8') as f:
    content = f.read()

parts = content.split('```mermaid')
for i in range(1, len(parts)):
    before_part = parts[i-1]
    headers = re.findall(r'###?\s+(.*)', before_part)
    if headers:
        title = headers[-1].strip()
        title = re.sub(r'[\\/*?:"<>|]', "_", title)
    else:
        title = f"Diagram_{i}"
        
    diagram_text = parts[i].split('```')[0].strip()
    
    print(f"Exporting {title}...")
    
    data = json.dumps({
      "diagram_source": diagram_text,
      "diagram_type": "mermaid",
      "output_format": "png"
    }).encode('utf-8')
    
    req = urllib.request.Request('https://kroki.io/', data=data, headers={
        'Content-Type': 'application/json',
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36'
    })
    try:
        with urllib.request.urlopen(req) as response:
            png_data = response.read()
            with open(os.path.join(out_dir, f"{title}.png"), 'wb') as img_f:
                img_f.write(png_data)
        print(f"Success: {title}.png")
    except Exception as e:
        if hasattr(e, 'read'):
            print(f"Failed to export {title}: {e} - {e.read().decode()}")
        else:
            print(f"Failed to export {title}: {e}")

print("Export completed.")
