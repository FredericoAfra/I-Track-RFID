<h1 align="center">
  🏭 IRFIDT
</h1>

<p align="center">
  <strong>Tracking and management system for materials and products in an industrial environment using RFID tags.</strong>
</p>

<p align="center">
  <img alt="Python" src="https://img.shields.io/badge/Python-3.11+-3776AB?style=flat&logo=python&logoColor=white"/>
  <img alt="FastAPI" src="https://img.shields.io/badge/FastAPI-0.100+-009688?style=flat&logo=fastapi&logoColor=white"/>
  <img alt="SQLite" src="https://img.shields.io/badge/SQLite-003B57?style=flat&logo=sqlite&logoColor=white"/>
  <img alt="ESP32" src="https://img.shields.io/badge/ESP32-RFID-E7352C?style=flat"/>
  <img alt="Status" src="https://img.shields.io/badge/status-em%20desenvolvimento-yellow?style=flat"/>
  <img alt="License" src="https://img.shields.io/badge/license-MIT-green?style=flat"/>
</p>

---

## 📋 Summary

- [About the project](#-about-the-project)
- [System Architeture](#-system-architeture)
- [Technology](#-system-architeture)
- [Arquitetura de Pastas](#-folder-structure)

---

## 📖 About the project

A system developed to track and manage the movement of materials and products within an industry. Each item is identified by an **RFID tag**, read by **RC522** sensors connected to **ESP32** microcontrollers installed at strategic points in the factory (entrances, exits, sectors, warehouse, etc.).

When a tag is read, the ESP32 sends the data to a **REST API** (FastAPI), which registers and updates the information in the database. A **web panel** consumes this same API to allow data management and the visualization of dashboards with movement analyses.

### How it works

```
[RFID tag]
    │
    ▼
[Sensor RC522 + ESP32]  ──→  [API FastAPI]  ──→  [SQLite Database]
    (point in factory)              │
                                    ▼
                                [Web page]
                         (dashboards + management)
```

### ✨ Preview Features

- [x] Database with tags tracking
- [ ] API REST for RFID read registers
- [ ] Web panel and management
- [ ] Dashboards and movement analysis
- [ ] Esp32 scripts
- [ ] Authentication and acess control

---

## 🏗 System Architeture

The project is a **monorepo** divided into two main parts — backend (under development) and frontend (pending):

| Layer | Technology | Status |
|---|---|---|
| Hardware | ESP32 + Sensor RC522 | ✅ Hardware defined |
| Backend / API | Python + FastAPI | 🔄 In development |
| Banco de Dados | SQLite | 🔄 In Development |
| Frontend / Painel | To set | ⏳ Pending |

---

## 🛠 Technology

| Component | Technology |
|---|---|
| Language | Python 3.11+ |
| Framework API | FastAPI |
| Database | SQLite |
| ORM / Queries | SQLAlchemy / aiosqlite |
| Data validation | Pydantic |
| Microcontroler | ESP32 |
| RFID sensor | RC522 |
| Comunication ESP32 → API | `⚠️ To set` |
| Frontend | `⚠️ To setr` |


---

## 📁 Folder structure

> The structure below reflects the current state of the project (backend) and the planned structure for the frontend.

```
IRFIDT/
│
├── Database/                       # database schema
|
├── frontend/                       # ⏳ Web panel
│
├── Firmware/                       # ⏳ Esp32 script
│                  
└── README.md
```
---

<p align="center">
  Made by <a href="https://github.com/FredericoAfra">Frederico</a>
</p>
